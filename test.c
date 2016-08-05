#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "gumbo.h"

static void read_file(FILE* fp, char** output, int* length) {
  struct stat filestats;
  int fd = fileno(fp);
  fstat(fd, &filestats);
  *length = filestats.st_size;
  *output = malloc(*length + 1);
  int start = 0;
  int bytes_read;
  while ((bytes_read = fread(*output + start, 1, *length - start, fp))) {
    start += bytes_read;
  }
}

struct row {
    int id;
    char* date;
    char* type;
    char* description;
    float ammount;
};

void parse_row(GumboNode* row) {
    printf("HANDLING ROW\n\n");
    for (unsigned int i = 0; i < row->v.element.children.length; i++) { //loop through cols
        GumboNode* child = row->v.element.children.data[i];
        if (child->type == GUMBO_NODE_ELEMENT) {
            int len = child->v.element.end_pos.offset - child->v.element.start_pos.offset;
            printf("%.*s\n", len, child->v.element.original_tag.data);
            printf("field chilren: %d\n", child->v.element.children.length);
            GumboNode* subc = child->v.element.children.data[0];
            printf("subc type: %d\n\n", subc->type);

            //printf("%s\n", child->v.element.attributes.data[0]);
            /*
            for (unsigned int j = 0; j < child->v.element.children.length; j++) {
                GumboNode* subc = child->v.element.children.data[j];
                int len = subc->v.element.end_pos.offset - subc->v.element.start_pos.offset;
                printf("%.*s\n", len, subc->v.element.original_tag.data);
            }*/


        }
    }
}

//TODO filter auths

static const char* find_table(const GumboNode* root) {
  assert(root->type == GUMBO_NODE_ELEMENT);
  assert(root->v.element.children.length >= 2);

  const GumboVector* root_children = &root->v.element.children;
  GumboNode* body = NULL;
  for (unsigned int i = 0; i < root_children->length; ++i) {
    GumboNode* child = root_children->data[i];
    if (child->type == GUMBO_NODE_ELEMENT &&
        child->v.element.tag == GUMBO_TAG_BODY) {
      body = child;
      break;
    }
  }
  assert(body != NULL);

  /*
  printf("node element %d\n", GUMBO_NODE_ELEMENT); //1
  printf("node text %d\n", GUMBO_NODE_TEXT); //2
  printf("node whitespace %d\n", GUMBO_NODE_WHITESPACE); //5
  printf("tag div %d\n", GUMBO_TAG_DIV); //38
  printf("tag table %d\n", GUMBO_TAG_TABLE);//95
  printf("tag p %d\n", GUMBO_TAG_P);//25
  printf("tag h %d\n", GUMBO_TAG_H1);
  printf("tag head %d\n", GUMBO_TAG_HEAD);
  printf("\n");

  for (unsigned int i = 0; i < body->v.element.children.length; i++) {
      GumboNode* child = body->v.element.children.data[i];
      if (child->type == GUMBO_NODE_ELEMENT)
          printf("body child %d   tag %d\n", i, child->v.element.tag);
  }
  printf("\n");
  */
  GumboNode* divcontent = body->v.element.children.data[7]; //<div id=content

  //printf("%s\n", divcontent->v.element.original_tag.data);

  GumboNode* divfeature = divcontent->v.element.children.data[5]; //<div id=content
  GumboNode* table = divfeature->v.element.children.data[1];

  GumboNode* tablecontent = table->v.element.children.data[1];

  /*
  GumboNode* row = tablecontent->v.element.children.data[26]; //should be 6-26

  printf("%s\n", row->v.element.original_tag.data);
  printf("\n");
  */

  //hardcoded 
  //filters all our nodes just to record rootes
  for (unsigned int i = 6; i < tablecontent->v.element.children.length - 5; i++) {
      GumboNode* child = tablecontent->v.element.children.data[i];
      if (child->type == GUMBO_NODE_ELEMENT) {
          //printf("%.*s\n", 100, child->v.element.original_tag.data);
          printf("\n\n\nchild %d   tag %d\n", i, child->v.element.tag);
          parse_row(child);
      }
  }

  exit(0);
}




/*
  GumboVector* head_children = &body->v.element.children;
  for (unsigned int i = 0; i < head_children->length; ++i) {
    GumboNode* child = head_children->data[i];
    if (child->type == GUMBO_NODE_ELEMENT &&
        child->v.element.tag == GUMBO_TAG_TITLE) {
      if (child->v.element.children.length != 1) {
        return "<empty title>";
      }
      GumboNode* title_text = child->v.element.children.data[0];
      assert(title_text->type == GUMBO_NODE_TEXT || title_text->type == GUMBO_NODE_WHITESPACE);
      return title_text->v.text.text;
    }
  }
  return "<no title found>";
}
*/

int main() {
    /*
  if (argc != 2) {
    printf("Usage: get_title <html filename>.\n");
    exit(EXIT_FAILURE);
  }
  const char* filename = argv[1];
  */
  const char* filename = "table.html";
  FILE* fp = fopen(filename, "r");
  if (!fp) {
    printf("File %s not found!\n", filename);
    exit(EXIT_FAILURE);
  }

  char* input;
  int input_length;
  read_file(fp, &input, &input_length);
  GumboOutput* output = gumbo_parse_with_options(
      &kGumboDefaultOptions, input, input_length);
  const char* title = find_table(output->root);
  printf("%s\n", title);
  gumbo_destroy_output(&kGumboDefaultOptions, output);
  free(input);
}



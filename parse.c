#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#include <gumbo.h>

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
    const char* date;
    const char* type;
    const char* description;
    float price;
};

struct row parse_row(GumboNode* row) {
    /* Take pointer to a record and parse out the fields into a struct
     *
     * Children:
     * 1 = id
     * 3 = date
     * 5 = type
     * 7 = description
     * 9 = price
     */

    struct row r;
    GumboVector* fields = &row->v.element.children;
    GumboNode* field;
    GumboNode* field_text;

    //id
    field = fields->data[1];
    field_text = field->v.element.children.data[0];
    r.id = atoi(field_text->v.text.text);
    printf("id: %d\n", r.id);

    //date
    field = fields->data[3];
    field_text = field->v.element.children.data[0];
    r.date = field_text->v.text.text;
    printf("date: %s\n", r.date);

    //type
    field = fields->data[5];
    field_text = field->v.element.children.data[0];
    r.type = field_text->v.text.text;
    printf("type: %s\n", r.type);


    //description
    field = fields->data[7];
    field_text = field->v.element.children.data[0];
    r.description = field_text->v.text.text;
    printf("description: %s\n", r.description);

    //price
    field = fields->data[9];
    field_text = field->v.element.children.data[0];
    GumboNode* price_div = field_text->v.element.children.data[0];
    char price[5];
    strncpy(price, (price_div->v.text.text + 2), 4);
    r.price = atof(price);
    printf("price: %f\n", r.price);

    return r;
}

/* //Research code
   for (unsigned int i = 0; i < fields->length; ++i) {
   GumboNode* field = fields->data[i];
   if (field->type == GUMBO_NODE_ELEMENT && field->v.element.tag == GUMBO_TAG_TD) {
//if (field->v.element.children.length != 1) {
//    printf( "<empty field>\n");
//    continue;
//}
GumboNode* field_text = field->v.element.children.data[0];
if (field_text->type == GUMBO_NODE_TEXT || field_text->type == GUMBO_NODE_WHITESPACE) //normal field
printf("field %d: %s\n", i, field_text->v.text.text);
else { //div, the price
GumboNode* price_div = field_text->v.element.children.data[0];
if (price_div->type == GUMBO_NODE_TEXT || price_div->type == GUMBO_NODE_WHITESPACE)
printf("danger: %s\n", price_div->v.text.text);
}
}
}
} */


struct row* find_table(const GumboNode* root) {
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

    /* //Research code
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
    GumboNode* divfeature = divcontent->v.element.children.data[5]; //<div id=content
    GumboNode* table = divfeature->v.element.children.data[1];
    GumboNode* tablecontent = table->v.element.children.data[1];

    
    struct row* rows = malloc(0);
    long rows_len = 0;
    //filters all our nodes just to record rootes //hardcoded 
    for (unsigned int i = 6; i < tablecontent->v.element.children.length - 5; i++) {
        GumboNode* child = tablecontent->v.element.children.data[i];
        if (child->type == GUMBO_NODE_ELEMENT) {
            //filter auths
            int len = child->v.element.end_pos.offset - child->v.element.start_pos.offset;
            if (strnstr(child->v.element.original_tag.data, "AUTH", len) != NULL) {
                printf("\n\n\nchild %d   tag %d\n", i, child->v.element.tag);
                rows = realloc(rows, (rows_len + 1) * sizeof(struct row));
                rows[rows_len] = parse_row(child);
                rows_len += 1;
            }
        }
    }
    return rows;
}

int main() {
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
    struct row* rows = find_table(output->root);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    free(input);
}



#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Exception.hpp>


namespace
{
    const long MyPort = 80;
}

/** 
 * This example is made to show you how you can use the Options.
 */
int main(int, char **)
{
    try
    {
        /*
        curlpp::Cleanup myCleanup;

        // First easy example.
        {
            // The first easiest example is to retreive the content of
            // a web page and put it in a stream.
            //std::cout << curlpp::options::Url("http://example.com");

            // You don't need to use just the standard outputs. You
            // can use any stream:
            std::ostringstream os;
            //os << curlpp::options::Url("http://example.com");
        }
        */

        // More elaborate example.
        {
            // What the previous example done there was simply 
            // to create a curlpp::Easy class, which is the basic
            // object in cURLpp, and then set the Url option.
            // curlpp::options classes are the primitives that allow to specify 
            // values to the requests. 
            std::string base = "https://bing.campuscardcenter.com/ch/login.html";
            curlpp::options::Url myUrl(base);

            curlpp::Forms formParts;
            formParts.push_back(new curlpp::FormParts::Content("username", "jfische8"));
            formParts.push_back(new curlpp::FormParts::Content("password", "hackbu"));

            std::list<std::string> headers;
            headers.push_back("Host: bing.campuscardcenter.com");
            headers.push_back("Origin: https://bing.campuscardcenter.com");
            headers.push_back("User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36");
            headers.push_back("Content-Type: application/x-www-form-urlencoded");
            headers.push_back("Referer: https://bing.campuscardcenter.com/ch/login.html");


            curlpp::Easy myRequest;
            myRequest.setOpt(myUrl);
            myRequest.setOpt(new curlpp::options::HttpPost(formParts));
            myRequest.setOpt(new curlpp::options::HttpHeader(headers));
            myRequest.setOpt(new curlpp::options::Cookie("DVH=89165847-a434-4e57-95f4-435ba89ada75"));
            

            myRequest.perform();

            // If we wanted to put the content of the URL within a string stream
            // (or any type of std::ostream, for that matter), like the first example, 
            // we would use the WriteStrem option like this:
            /*********
            std::ostringstream os;
            curlpp::options::WriteStream ws(&os);
            myRequest.setOpt(ws);
            myRequest.perform();
            */

            std::list<std::string> cookies;
            curlpp::infos::CookieList::get(myRequest, cookies);
            
            // There is some shorcut within curlpp that allow you to write shorter code
            // like this:
            for (auto c:cookies)
                std::cout << c <<std::endl;

            // That would do exactly what the previous code was doing.
        }

        /*
        // Creation of the URL option.
        curlpp::options::Url myUrl(std::string("http://example.com"));

        // Copy construct from the other URL.
        curlpp::options::Url myUrl2(myUrl);

        // Creation of the port option.
        curlpp::options::Port myPort(MyPort);

        // Creation of the request.
        curlpp::Easy myRequest;

        // Creation of an option that contain a copy of the URL option.
        curlpp::OptionBase *mytest = myUrl.clone();
        myRequest.setOpt(*mytest);

        // You can reuse the base option for other type of option
        // and set the option to the request. but first, don't forget 
        // to delete the previous memory. You can delete it since the 
        // option is internally duplicated for the request.
        delete mytest;
        mytest = myPort.clone();
        myRequest.setOpt(*mytest);
        delete mytest;

        // You can clone an option directly to the same type of 
        // option.
        curlpp::options::Url *myUrl3 = myUrl.clone();
        myRequest.setOpt(myUrl3);
        // Now myUrl3 is owned by the request we will NOT use 
        // it anymore.


        // You don't need to declare an option if you just want 
        // to use it once.
        myRequest.setOpt(curlpp::options::Url("example.com"));


        // Note that the previous line wasn't really efficient
        // because we create the option, this option is duplicated
        // for the request and then the option destructor is called.
        // You can use this instead:
        myRequest.setOpt(new curlpp::options::Url("example.com"));
        // Note that with this the request will use directly this
        // instance we just created. Be aware that if you pass an
        // Option pointer to the setOpt function, it will consider
        // the instance has its own instance. The Option instance
        // will be deleted when the request will be deleted, so
        // don't use the instance further in your code.


        // Doing the previous line is efficient as this:
        myRequest.setOpt(myUrl.clone());


        // You can retreive the value of a specific option.
        std::cout << myUrl2.getValue() << std::endl;

        // Perform the transaction with the options set.
        myRequest.perform();
        */
    }

    catch( curlpp::RuntimeError &e )
    {
        std::cout << e.what() << std::endl;
    }

    catch( curlpp::LogicError &e )
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Exception.hpp>


std::vector<std::string> extractCookies(std::string c)
{
    std::vector<std::string> result;
    std::string cookie_start = "Set-Cookie: ";
    size_t pos = c.find(cookie_start);
    while (pos != std::string::npos) {
        size_t temp = pos;
        pos = c.find(";", temp);
        temp += cookie_start.length();
        std::string contents = c.substr(temp, pos-temp);
        result.push_back(contents);
        pos = c.find(cookie_start, pos);
    }
    return result;
}


int main(int, char **)
{
    try
    {
        curlpp::Cleanup myCleanup;

        {
            std::string base = "https://bing.campuscardcenter.com/ch/login.html";
            //base = "http://jackfischer.me/test.php";

            std::list<std::string> headers;
            //headers.push_back("Host: bing.campuscardcenter.com");
            headers.push_back("Origin: https://bing.campuscardcenter.com");
            headers.push_back("User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36");
            headers.push_back("Referer: https://bing.campuscardcenter.com/ch/login.html");
            headers.push_back("Upgrade-Insecure-Requests: 1");
            headers.push_back("Content-Type: application/x-www-form-urlencoded");
            headers.push_back("Connection: keep-alive");
            //headers.push_back("Accept-Encoding: gzip, deflate, br");

            curlpp::Easy myRequest;
            myRequest.setOpt(new curlpp::options::Url(base));
            myRequest.setOpt(new curlpp::options::PostFields("username=jfische8&password=hackbu&action=Login"));
            myRequest.setOpt(new curlpp::options::HttpHeader(headers));
            myRequest.setOpt(new curlpp::options::Cookie("DVH=89165847-a434-4e57-95f4-435ba89ada75"));

            //myRequest.setOpt(new curlpp::options::NoBody(true)); 
            myRequest.setOpt(new curlpp::options::Header(true)); 


            std::stringstream os;
            curlpp::options::WriteStream ws(&os);
            myRequest.setOpt(ws);


            myRequest.perform();
            std::cout << "request performed." <<std::endl;
            std::vector<std::string> cookies = extractCookies(os.str());
            for (std::string s : cookies)
                std::cout << s <<std::endl;

        }

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

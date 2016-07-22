#include <chrono>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
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
            std::string login = "https://bing.campuscardcenter.com/ch/login.html";
            //login = "http://jackfischer.me/test.php";

            /* GET LOGIN PAGE FOR DVH COOKIE */
            std::list<std::string> headers;
            headers.push_back("Origin: https://bing.campuscardcenter.com");
            headers.push_back("User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36");
            headers.push_back("Referer: https://bing.campuscardcenter.com/ch/login.html");
            headers.push_back("Upgrade-Insecure-Requests: 1");
            headers.push_back("Connection: keep-alive");

            curlpp::Easy getLogin;
            getLogin.setOpt(new curlpp::options::Url(login));
            getLogin.setOpt(new curlpp::options::HttpHeader(headers));
            getLogin.setOpt(new curlpp::options::Header(true)); 

            std::stringstream os;
            curlpp::options::WriteStream ws(&os);
            getLogin.setOpt(ws);
            getLogin.perform();
            std::cout << "performed initial getLogin" <<std::endl;

            std::vector<std::string> cookies = extractCookies(os.str());
            for (std::string s : cookies)
                std::cout << s <<std::endl;
            std::cout <<"ok, sleeping" <<std::endl <<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            
            

            /* POST TO LOGIN PAGE WITH CREDENTIALS, GET JSESSIONID COOKIE*/
            curlpp::Easy doLogin;
            doLogin.setOpt(new curlpp::options::Url(login));
            doLogin.setOpt(new curlpp::options::PostFields("username=jfische8&password=hackbu&action=Login"));
            headers.push_back("Content-Type: application/x-www-form-urlencoded");
            doLogin.setOpt(new curlpp::options::HttpHeader(headers));
            for (std::string s : cookies)
                doLogin.setOpt(new curlpp::options::Cookie(s));

            //myRequest.setOpt(new curlpp::options::NoBody(true)); 
            doLogin.setOpt(new curlpp::options::Header(true)); 


            std::stringstream os2;
            curlpp::options::WriteStream ws2(&os2);
            doLogin.setOpt(ws2);
            doLogin.perform();
            std::cout << "performed doLogin post request" <<std::endl;
            std::cout << os2.str() <<std::endl;
            std::vector<std::string> cookies2 = extractCookies(os2.str());
            for (std::string s : cookies2)
                std::cout << s <<std::endl;
            std::cout <<"ok, sleeping" <<std::endl <<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
             
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

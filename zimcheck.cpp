#include <zim/file.h>
#include <zim/fileiterator.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
std::vector <std::string> get_links(std::string page)           //Returns a vector of the links in a particular page. includes links under 'href' and 'src'
{
    std::vector <std::string> links;
    for(int i=0;i<page.size();i++)
    {
        if(page[i]==' '&&i+5<page.size())
        {
            if(page[i+1]=='h'&&page[i+2]=='r'&&page[i+3]=='e'&&page[i+4]=='f')      //Links under 'href' category.
            {
                i+=5;
                while(page[i]!='=')
                    i++;
                while(page[i]!='"')
                    i++;
                i++;
                std::string link="";
                while(page[i]!='"')
                {
                    link+=page[i];
                    i++;
                }
                links.push_back(link);
            }

            if(page[i+1]=='s'&&page[i+2]=='r'&&page[i+3]=='c')      //Links under 'src' category.
            {
                i+=4;
                while(page[i]!='=')
                    i++;
                while(page[i]!='"')
                    i++;
                i++;
                std::string link="";
                while(page[i]!='"')
                {
                    link+=page[i];
                    i++;
                }
                links.push_back(link);
            }
        }
    }
    return links;
}

std::string parse_url(std::string url)            //Function to parse a url and to return the domain name.
{
    std::vector<std::string> link;
    link.clear();
    int counter=0;
    for(int i=0;i<url.size();i++)
    {
        if (url[i]=='.')
            counter++;
    }
    link.resize(counter);
    int i=0;
    int pos=0;
    for(i=0;i<counter;i++)
    {
        link[i]="";
    }
    i=0;
    while(i<counter)
    {
        if(url[pos]!='.')
            link[i]+=(char)url[pos];
        else
        {
            i++;
        }
        pos++;
    }
    std::string op;
    if(link.size()>2)
        op=link[1];
    link.clear();
    return op;
}
bool is_wikipedia_url(std::string input)
{
    std::string op=parse_url(input);
    if(op=="wikipedia")
        return true;
    else
        return false;
}
bool verify_url(zim::File f,std::string url)        //Returns if a URL is present in a file.
{
    for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
    {
        if(url==it->getUrl())
            return true;
    }
    return false;
}

int checkData(zim::File f,std::string page)
{
    int count=0;
    for(zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
    {
        if(page==it->getPage())
            count++;
    }
    return count;
}

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
       std::cerr << " Error opening File.\n";
       std::cerr << " Help: Zimcheck\n";
       std::cerr << " A tool to check the quality of a ZIM file.\n";
       std::cerr << " Written by : Kiran Mathew Koshy\n";
       return 0;
    }
	try
	{
   		zim::File f(argv[1]);
		std::cout<<"\nRunning Tests...";

		//Test 1: Internal Checksum
		std::cout<<"\nTest 1: Internal Checksum: ";
        if(f.verify())
            std::cout<<"Pass\n";
        else
        {
            std::cout<<"Fail\n";
            return 0;
        }



        //Test 2: Dead internal URLs
        std::cout<<"\nTest 2: Dead Internal URLs : NOT IMPLEMENTED YET\n";
        std::vector<std::string> output;
        for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
        {
            if(it->getMimeType()=="text/html")
            {
                output=get_links(it->getPage());
                for(int i=0;i<output.size();i++)
                {
                    ;
                }
            }
        }

        //Test 3: External URLs:
        std::cout<<"\nTest 3: External URLs : ";
        output.clear();
        bool pass=true;
        for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
        {
            if(it->getMimeType()=="text/html")
            {
                output.clear();
                output=get_links(it->getPage());
                for(int i=0;i<output.size();i++)
                {
                     bool check=is_wikipedia_url(output[i]);
                     if(!check)
                        pass=false;
                }
            }
        }
        if(!pass)
            std::cout<<"Fail. \n";
        else
            std::cout<<"Pass. \n";

        //Test 4: Redundant data:
        pass=true;
        std::cout<<"\nTest 4: Redundant Data : ";
        output.clear();
        for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
        {
            std::string page=it->getPage();
            int count=checkData(f,page);
            if(count>1)
            {
                pass=false;
            }
        }
        if(!pass)
            std::cout<<"Fail. \n";
        else
            std::cout<<"Pass. \n";



        //Test 5: NOT IMPLEMENTED YET.
        std::cout<<"\nTest 5: NOT IMPLEMENTED YET : ";
        std::cout<<"\n\n";



        //Test 6: NOT IMPLEMENTED YET.
        std::cout<<"\nTest 6: NOT IMPLEMENTED YET : ";
        std::cout<<"\n\n";



        //Test 7: NOT IMPLEMENTED YET.
        std::cout<<"\nTest 7: NOT IMPLEMENTED YET : ";
        std::cout<<"\n\n";



        //Test 8: NOT IMPLEMENTED YET.
        std::cout<<"\nTest 8: NOT IMPLEMENTED YET : ";
        std::cout<<"\n\n";



        //Test 9: NOT IMPLEMENTED YET.
        std::cout<<"\nTest 9: NOT IMPLEMENTED YET : ";
        std::cout<<"\n\n";


	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

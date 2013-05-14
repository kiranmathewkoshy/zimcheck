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

void parse_url(std::string url)            //Function to parse a url and to return the domain name.
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
    for(int i=0;i<counter;i++)
    {
        std::cout<<std::endl<<i<<" . "<<link[i];
    }
    link.clear();
    return ;
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

	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

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
        std::cout<<"\n"<<i<<" . "<<link[i];
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
    std::string filename="wikipedia.zim";
    if(argc==2)
        filename=argv[1];
    std::vector<std::string> title_list;
    std::string temp;
	try
	{
   		zim::File f(filename);
        std::cout<<"Reading ZIM file...\n";
		for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
		{
            title_list.push_back(it->getTitle());
		}
		std::cout<<title_list.size()<<" articles were read from the ZIM file.\n";

		std::cout<<"Searching for duplicates..\n";
		for(int i=0;i<title_list.size();i++)
		{
            for(int j=0;j<title_list.size()-1;j++)
            {
                if(title_list[j]>title_list[j+1])
                {
                    temp=title_list[j];
                    title_list[j]=title_list[j+1];
                    title_list[j+1]=temp;
                }
            }
        }
        bool found=false;
        for(int i=0;i<title_list.size()-1;i++)
        {
            if(title_list[i]==title_list[i+1])
                std::cout<<"Duplicate Entry found\n";
        }
        if(!found)
            std::cout<<"No duplicates entries found\n";

        for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
		{
            if(it->getMimeType()=="text/html")
            {
                std::vector<std::string> s=get_links(it->getPage());
                int ct=0;
                for(int i=0;i<s.size();i++)
                {
                    bool result= verify_url(f,s[i]);
                    parse_url(s[i]);
                    if(!result)
                    {
                        ct++;
                        std::cout<<"\nExternal URl: "<<s[i]<<"\n";
                        //getchar();
                    }
                }
                s.clear();

                std::cout<<"\n"<<it->getUrl()<<" : "<<s.size()<<" links found; "<<ct<<" are external";
            }
		}
        std::cout<<"\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

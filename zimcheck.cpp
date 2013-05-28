/*
 * Copyright (C) 2012 Kiran mathew Koshy
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */
#include <zim/file.h>
#include <zim/fileiterator.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <vector>
#include "arg.h"
#include <list>
#include <regex>
std::vector <std::string> get_links(std::string page)           //Returns a vector of the links in a particular page. includes links under 'href' and 'src'
{
    std::vector <std::string> links;
    for(int i=0; i<page.size(); i++)
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
    for(int i=0; i<url.size(); i++)
    {
        if (url[i]=='.')
            counter++;
    }
    link.resize(counter);
    int i=0;
    int pos=0;
    for(i=0; i<counter; i++)
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
class article_index                                 //Stores the article index and the hash.
{
public:
    unsigned int hash_;
    int index;
    bool operator<(const article_index a)
    {
        return hash_<a.hash_?true:false;

    }

};

class progress_bar
{
    private:
    char icon;
    int max_icons;
    int max_no;
    int no;
    int counter;
    int is_initialised;
    double displayed,count_max;
    public:
    progress_bar(char icon_,int max_n)
    {
        if(max_n<1)
        {
            is_initialised=-1;
            return;
        }
        if(max_n<1)
        {
            is_initialised=-1;
            return;
        }
        is_initialised=1;
        max_icons=80;
        icon=icon_;
        max_no=max_n;
        no=0;
        counter=0;
        count_max=0.0;
        displayed=0.0;
        return;
    }
    void report()
    {
        if(is_initialised!=1)
            return;
        counter++;
        displayed=(no+1)/80.00;
        count_max=(counter*1.0)/max_no;
        if(count_max>displayed)
        {
            std::cout<<icon<<std::flush;
            no++;
        }
        if(no>=80)
            is_initialised=-1;
        return;
    }
    void initialise(char icon_,int max_n)
    {
        if(max_n<1)
        {
            is_initialised=-1;
            return;
        }
        if(max_n<1)
        {
            is_initialised=-1;
            return;
        }
        is_initialised=1;
        max_icons=80;
        icon=icon_;
        max_no=max_n;
        no=0;
        counter=0;
        count_max=0.0;
        displayed=0.0;
        return;
    }
};

class int_pair
{
    public:
    int a;
    int b;
};
//Adler32 Hash Function
int adler32(std::string buf)
{
     unsigned int s1 = 1;
     unsigned int s2 = 0;

     for (size_t n = 0; n <buf.size(); n++)
     {
        s1 = (s1 + buf[n]) % 65521;
        s2 = (s2 + s1) % 65521;
     }
     return (s2 << 16) | s1;
}

bool is_external_url(std::string s)
{
    if(std::regex_match(s,std::regex("(http://)(.*)")))
        return true;

    if(std::regex_match(s,std::regex("(https://)(.*)")))
        return true;
}

bool is_external_wikipedia_url(std::string s)
{
    if(std::regex_match(s,std::regex("(http://)(.*)")))
        return true;

    if(std::regex_match(s,std::regex("(https://)(.*)")))
        return true;
}

bool is_internal_url(std::string s)
{
    if(std::regex_match(s,std::regex("(/)(.)(/)(.*)")))
        return true;
    else
        return false;
}

int main(int argc, char* argv[])
{

    //Processing Flags passed to the program.
    zim::Arg<bool> run_all(argc, argv, 'A');
    zim::Arg<bool> checksum(argc, argv, 'C');
    zim::Arg<bool> metadata(argc, argv, 'M');
    zim::Arg<bool> favicon(argc, argv, 'F');
    zim::Arg<bool> main_page(argc, argv, 'P');
    zim::Arg<bool> redundant_data(argc, argv, 'R');
    zim::Arg<bool> url_check(argc, argv, 'U');
    zim::Arg<bool> mime_check(argc, argv, 'E');
    progress_bar progress('#',10);
    std::cout<<"\n"<<argv[argc-1]<<std::flush;
    if (argc <= 1)
    {
        std::cerr << "usage: " << argv[0] << " [options] zimfile\n"
                   "\n"
                   "options:\n"
                   "  -A        run all tests. Default if no flags are given.\n"
                   "  -C        Internal CheckSum Test\n"
                   "  -M        MetaData Entries\n"
                   "  -F        Favicon\n"
                   "  -P       Main page\n"
                   "  -R        Redundant data check\n"
                   "  -U        URL checks\n"
                   "  -E       MIME checks\n"
                   "\n"
                   "examples:\n"
                   "  " << argv[0] << " -A wikipedia.zim\n"
                   "  " << argv[0] << " -C wikipedia.zim\n"
                   "  " << argv[0] << " -F -R wikipedia.zim\n"
                   "  " << argv[0] << " -MI wikipedia.zim\n"
                   "  " << argv[0] << " -U wikipedia.zim\n"
                   "  " << argv[0] << " -R -U wikipedia.zim\n"
                   "  " << argv[0] << " -R -U -MI wikipedia.zim\n"
                 << std::flush;
        return -1;
    }
    try
    {
        bool test_=false;
        zim::File f(argv[argc-1]);
        std::cout<<"\nRunning Tests...";
        int c=0;
        for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
            c++;
        progress.initialise('#',c);
        //Test 1: Internal Checksum
        if(run_all||checksum)
        {
            std::cout<<"\nTest 1: Internal Checksum: ";
            if(f.verify())
                std::cout<<"Pass\n";
            else
            {
                std::cout<<"Fail\n";
            }
        }


        //Test 2: Dead internal URLs
        std::cout<<"\nTest  2: Metadata Entries: ";
        bool test_meta[6];
        for(int i=0; i<6; i++)
            test_meta[i]=false;
        for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
        {
            if(it->getNamespace()=='M')
            {
                if(it->getTitle()=="Title")
                    test_meta[0]=true;
                if(it->getTitle()=="Creator")
                    test_meta[1]=true;
                if(it->getTitle()=="Publisher")
                    test_meta[2]=true;
                if(it->getTitle()=="Date")
                    test_meta[3]=true;
                if(it->getTitle()=="Description")
                    test_meta[4]=true;
                if(it->getTitle()=="Language")
                    test_meta[5]=true;
            }
        }
        test_=true;
        for(int i=0; i<6; i++)
            if(!test_meta[i])
                test_=false;
        if(test_)
            std::cout<<"Pass\n";
        else
        {
            std::cout<<"Fail\n";
        }


        //Test 3: Test for Favicon.
        std::cout<<"\nTest 3: Test for Favicon: ";
        test_=false;
        for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
        {
            if(it->getNamespace()=='-')
                if(it->getTitle()=="favicon.png")
                    test_=true;
        }
        if(test_)
        {
            std::cout<<"Pass\n";
        }
        else
        {
            std::cout<<"Fail\n";
        }


        //Test 4: Main Page Entry
        test_=true;
        std::cout<<"\nTest 4: Main Page Entry: ";
        zim::Fileheader fh=f.getFileheader();
        if(!fh.hasMainPage())
            test_=false;
        else
        {
            if(fh.getMainPage()>fh.getArticleCount())
                test_=false;
        }
        if(test_)
            std::cout<<"Pass\n";
        else
        {
            std::cout<<"Fail\n";
        }

        std::cout<<"\nTest 5: Redundant data: "<<std::flush;

        test_=false;
        int max=0;
        int k;
        std::cout<<"\nCreating Data Structures...\n"<<std::flush;
        for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
        {
            k=it->getArticleSize();
            if(k>max)
                max=k;
            progress.report();
        }
        //Data Storage system
        std::vector<std::list<article_index> >hash_main;

        //Allocating Double Hash Tree.
        hash_main.resize(max+1);

        //List of Articles to be compared against
        article_index article;

        //Adding data to hash Tree.
        std::cout<<"\nAdding Data to Hash Tables from file...\n"<<std::flush;
        int i=0;
        char arr[100000];
        std::string ar;
        zim::Blob bl;
        progress.initialise('#',c);
        int sz=0;
        for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
        {
            bl=it->getData();
            sz=bl.size();
            ar.clear();
            for(int i=0;i<sz;i++)
                ar+=bl.data()[i];
            article.hash_ = adler32(ar);
            article.index=i;
            hash_main[ar.size()].push_back(article);
            i++;
            progress.report();
        }
        //Checking through hash tree for redundancies.
        //Sorting the hash tree.
        std::cout<<"\nSorting Hash Tree...\n"<<std::flush;
        int hash_main_size=hash_main.size();
        progress.initialise('#',c);
        for(int i=0;i<hash_main_size;i++)
        {
            hash_main[i].sort();
            progress.report();
        }
        std::vector<int_pair> to_verify;
        //Processing the tree
        std::cout<<"\nSearching for redundant Data...\n"<<std::flush;
        progress.initialise('#',c);
        for(int i=0;i<hash_main_size;i++)
        {
            std::list<article_index>::iterator it=hash_main[i].begin();
            article_index prev;
            prev.index=it->index;
            prev.hash_=it->hash_;
            ++it;
            for (; it != hash_main[i].end(); ++it)
            {
                if(it->hash_==prev.hash_)
                {
                    int_pair p;
                    p.a=it->index;
                    p.b=prev.index;
                    to_verify.push_back(p);
                }
                prev.index=it->index;
                prev.hash_=it->hash_;
            }
            progress.report();
        }
        test_=true;
        std::cout<<"\nVerifying Similar Articles for redundancies.. \n"<<std::flush;
        progress.initialise('#',c);
        for(int i=0;i<to_verify.size();i++)
        {
            bool op=false;
            zim::File::const_iterator it = f.begin();
            std::string s1,s2;
            for(int k=0;k<to_verify[i].a;k++)
            ++it;
            s1=it->getPage();
            it = f.begin();
            for(int k=0;k<to_verify[i].b;k++)
            ++it;
            s2=it->getPage();
            if(s1==s2)
                test_=false;
            progress.report();
        }
        if(test_)
            std::cout<<"Pass\n";
        else
        {
            std::cout<<"Fail\n";
        }



        //Test 6: Checking Internal URLs

        std::cout<<"\nTest 6: Verifying Internal URLs.. \n"<<std::flush;
        progress.initialise('#',c);
        test_=true;
        for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
        {
            if(it->getMimeType()=="text/html")
            {
                std::vector<std::string> links=get_links(it->getPage());
                for(int i=0;i<links.size();i++)
                {
                    if(!is_internal_url(links[i]))
                        test_=false;
                }
            }
            progress.report();
        }
        if(test_)
            std::cout<<"\nPass\n";
        else
        {
            std::cout<<"\nFail\n";
        }

        //Test 6: Verifying MIME Types
        /*
        std::cout<<"\nTest 7: Verifying MIME Types.. \n"<<std::flush;
        progress.initialise('#',c);
        test_=true;
        for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
        {

        }
        */
        std::cout<<"\n"<<f.getFileheader().getMimeListPos()<<"check"<<std::flush;
        //Test 5: Check for Absolute Internal URLs
        /*
        test_=true;
        std::cout<<"\nTest 5: Absolute internal URLs:";
        for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
        {
            std::vector<std::string> data;
            data=get_links(it->getPage());
            for(int i=0;i<data.size();i++)
                std::cout<<"\n"<<data[i];
        }
        std::cout<<"\n";


        */

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

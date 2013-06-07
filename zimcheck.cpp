#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <zim/file.h>
#include <zim/fileiterator.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <vector>
#include "arg.h"
#include <list>
#include <algorithm>
#include <regex>
#include <cstring>
#include <unistd.h>

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

class article_title_url
{
public:
    std::string title;
    std::string url;
    bool operator<(const article_title_url a)
    {
        return title<a.title?true:false;
    }

    bool operator== (const article_title_url a)
    {
        return title==a.title?true:false;
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
        float i=(counter*1.0)/max_no;
        float disp=(no*1.0)/max_icons;
        while((disp<i)&&no<max_icons)
        {
            disp=(no*1.0)/max_icons;
            no++;
            std::cout<<icon<<std::flush;
        }
        if(no>=max_icons)
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
    void initialise(char icon_,int max_n,int max_ic)
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
        if(max_ic<1)
        {
            is_initialised=-1;
            return;
        }
        max_icons=max_ic;
        is_initialised=1;
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

bool is_external_wikipedia_url(std::string s)
{
    if(std::regex_match(s,std::regex("(http://en.wikipedia.org/)(.*)")))
        return true;

    if(std::regex_match(s,std::regex("(https://en.wikipedia.org/)(.*)")))
        return true;

    if(std::regex_match(s,std::regex("(en.wikipedia.org/)(.*)")))
        return true;

    if(std::regex_match(s,std::regex("(.*)(.wikipedia.org/)(.*)")))
        return true;

    if(std::regex_match(s,std::regex("(.*)(.wikimedia.org/)(.*)")))
        return true;

    return false;
}

bool is_internal_url(std::string s)
{
    if(std::regex_match(s,std::regex("(/)(.)(/)(.*)")))
        return true;
    else
        return false;
}

bool compare_article_titles(article_title_url a, article_title_url b)
{
    return a.title<b.title?true:false;
}

std::string to_string(int a)
{
    std::string to_return;
    to_return.clear();
    char temp;
    while(a>0)
    {
        temp='0'+a%10;
        to_return+=temp;
        a/=10;
    }
    return to_return;
}

std::string process_links(std::string input)
{
    std::string output;
    output.clear();
    int pos=0;
    while(pos<input.size())
    {
        if(input[pos]!='%')
        {
            output+=input[pos];
        }
        else
        {
            pos+=2;
            output+=' ';
        }
        pos++;
        if(input[pos]=='#')
            return output;
    }
    return output;
}

std::string process_links_2(std::string input)          //Removes double or triple spaces from URLs.
{
    std::string output;
    output.clear();
    int pos=0;
    while(pos<input.size())
    {
        if(input[pos]==' ')
        {
            while((input[pos+1]==' ')&&(pos<input.size()))
                pos++;
        }
        output+=input[pos];
        pos++;
    }
    return output;
}

int main (int argc, char **argv)
{
    //Processing Flags passed to the program.

    bool run_all=false;
    bool checksum=false;
    bool metadata=false;
    bool favicon=false;
    bool main_page=false;
    bool redundant_data=false;
    bool url_check=false;
    bool url_check_external=false;
    bool mime_check=false;
    bool error_details=false;
    bool no_args=false;
    int aflag = 0;
    int bflag = 0;
    int index;
    int c;
    progress_bar progress('#',10);
    opterr = 0;
    if (argc <= 1)
    {
        std::cerr << "usage: " << argv[0] << " [options] zimfile\n"
                  "\n"
                  "options:\n"
                  "  -A        run all tests. Default if no flags are given.\n"
                  "  -C        Internal CheckSum Test\n"
                  "  -M        MetaData Entries\n"
                  "  -F        Favicon\n"
                  "  -P        Main page\n"
                  "  -R        Redundant data check\n"
                  "  -U        URL checks\n"
                  "  -X        External Dependency check\n"
                  "  -E        MIME checks\n"
                  "  -D        Lists Details of the errors in the ZIM file.\n"
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
    while ((c = getopt (argc, argv, "ACMFPRUXED")) != -1)
        switch (c)
        {
        case 'A':
            run_all = true;
            break;
        case 'C':
            checksum = true;
            break;
        case 'M':
            metadata = true;
            break;
        case 'F':
            favicon = true;
            break;
        case 'P':
            main_page = true;
            break;
        case 'R':
            redundant_data = true;
            break;
        case 'U':
            url_check = true;
            break;
        case 'X':
            url_check_external = true;
            break;
        case 'E':
            mime_check = true;
            break;
        case 'D':
            error_details = true;
            break;
        case '?':
            if (optopt == 'c')
                std::cerr<<"Option "<<(char)optopt<<" requires an argument.\n"
                         "options:\n"
                         "  -A        run all tests. Default if no flags are given.\n"
                         "  -C        Internal CheckSum Test\n"
                         "  -M        MetaData Entries\n"
                         "  -F        Favicon\n"
                         "  -P        Main page\n"
                         "  -R        Redundant data check\n"
                         "  -U        URL checks\n"
                         "  -X        External Dependency check\n"
                         "  -E        MIME checks\n"
                         "  -D        Lists Details of the errors in the ZIM file.\n"
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
            else if (isprint (optopt))
                std::cerr<<"Unknown option `"<<optopt<<"'.\n";
            else
                std::cerr<<"Unknown option character `\\x"<<optopt<<"'.\n";
            return 1;
        default:
            abort ();
        }

    if((run_all||checksum||metadata||favicon||main_page||redundant_data||url_check||mime_check)==false)
        no_args=true;


    try
    {
        bool test_=false;
        zim::File f(argv[argc-1]);
        std::cout<<"\nRunning Tests...";
        int c=0;
        for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
            c++;
        //Test 1: Internal Checksum
        if(run_all||checksum||no_args)
        {
            std::cout<<"\nTest 1: Internal Checksum: ";
            if(f.verify())
                std::cout<<"Pass\n";
            else
            {
                std::cout<<"Fail\n";
                if(error_details)
                {
                    std::cout<<"Details: \n";
                    std::cout<<"ZIM File Checksum: "<<f.getChecksum()<<"\n";
                }
            }
        }


        //Test 2: Metadata Entries:
        if(run_all||metadata||no_args)
        {
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
                if(error_details)
                {
                    std::cout<<"Details: \n";
                    if(!test_meta[0])
                        std::cout<<"Title not found in Metadata\n";
                    if(!test_meta[1])
                        std::cout<<"Creator not found in Metadata\n";
                    if(!test_meta[2])
                        std::cout<<"Publisher not found in Metadata\n";
                    if(!test_meta[3])
                        std::cout<<"Date not found in Metadata\n";
                    if(!test_meta[4])
                        std::cout<<"Description not found in Metadata\n";
                    if(!test_meta[5])
                        std::cout<<"Language not found in Metadata\n";
                }
            }
        }


        //Test 3: Test for Favicon.
        if(run_all||favicon||no_args)
        {
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
        }


        //Test 4: Main Page Entry
        if(run_all||main_page||no_args)
        {
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
                if(error_details)
                {
                    std::cout<<"Main Page Index stored in File Header: "<<fh.getMainPage()<<"\n";
                }
            }
        }


        //Test 5: Redundant Data:
        if(run_all||redundant_data||no_args)
        {
            std::cout<<"\nTest 5: Redundant data: \n"<<std::flush;

            test_=false;
            int max=0;
            int k;
            //std::cout<<"\nCreating Data Structures...\n"<<std::flush;
            progress.initialise('#',c,16);
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
            //std::cout<<"\nAdding Data to Hash Tables from file...\n"<<std::flush;
            int i=0;
            char arr[100000];
            std::string ar;
            zim::Blob bl;
            progress.initialise('#',c,16);
            int sz=0;
            for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
            {
                bl=it->getData();
                sz=bl.size();
                ar.clear();
                for(int i=0; i<sz; i++)
                    ar+=bl.data()[i];
                article.hash_ = adler32(ar);
                article.index=i;
                hash_main[ar.size()].push_back(article);
                i++;
                progress.report();
            }
            //Checking through hash tree for redundancies.
            //Sorting the hash tree.
            //std::cout<<"\nSorting Hash Tree...\n"<<std::flush;
            int hash_main_size=hash_main.size();
            progress.initialise('#',hash_main_size,16);
            for(int i=0; i<hash_main_size; i++)
            {
                hash_main[i].sort();
                progress.report();
            }
            std::vector<int_pair> to_verify;
            //Processing the tree
            //std::cout<<"\nSearching for redundant Data...\n"<<std::flush;
            progress.initialise('#',hash_main_size,16);
            for(int i=0; i<hash_main_size; i++)
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
                        if(it->index!=prev.index)
                        {
                            p.a=it->index;
                            p.b=prev.index;
                            to_verify.push_back(p);
                        }
                    }
                    prev.index=it->index;
                    prev.hash_=it->hash_;
                }
                progress.report();
            }
            test_=true;
            //std::cout<<"\nVerifying Similar Articles for redundancies.. \n"<<std::flush;
            progress.initialise('#',to_verify.size(),16);
            std::string output_details;
            for(int i=0; i<to_verify.size(); i++)
            {
                bool op=false;
                zim::File::const_iterator it = f.begin();
                std::string s1,s2;
                for(int k=0; k<to_verify[i].a; k++)
                    ++it;
                s1=it->getPage();
                it = f.begin();
                for(int k=0; k<to_verify[i].b; k++)
                    ++it;
                s2=it->getPage();
                if(s1==s2)
                {
                    test_=false;
                    output_details+="\nArticles ";
                    output_details+=to_string(to_verify[i].a);
                    output_details+=" and ";
                    output_details+=to_string(to_verify[i].b);
                    output_details+=" have the same content";
                }
                progress.report();
            }
            if(test_)
                std::cout<<"\nPass\n";
            else
            {
                std::cout<<"\nFail\n";
                if(error_details)
                    std::cout<<"Details: "<<output_details;
            }
        }

        //Test 6: Verifying Internal URLs
        if(run_all||url_check||no_args)
        {
            std::cout<<"\nTest 6: Verifying Internal URLs : \n"<<std::flush;
            std::vector < std::vector<article_title_url> >titles;
            titles.resize(256);
            article_title_url ar;
            for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
            {
                ar.title=it->getTitle();
                ar.url=it->getUrl();
                titles[(int)it->getNamespace()].push_back(ar);
            }
            for(int i=0; i<256; i++)
            {
                std::sort(titles[i].begin(),titles[i].end(),compare_article_titles);
            }
            progress.initialise('#',c);
            test_=true;
            std::string output_details;
            int ct=0;
            for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
            {
                if(it->getMimeType()=="text/html")
                {
                    std::vector<std::string> links=get_links(it->getPage());
                    for(int i=0; i<links.size(); i++)
                    {
                        links[i]=process_links(links[i]);
                        links[i]=process_links_2(links[i]);
                        if(is_internal_url(links[i]))
                        {
                            ar.title=links[i].substr(3,std::string::npos);
                            bool found=false;
                            int nm=(int)links[i][1];
                            if(std::binary_search(titles[nm].begin(),titles[nm].end(),ar,compare_article_titles))
                                found=true;
                            if(!found)
                            {
                                if(error_details)
                                {
                                    output_details+="\nArticle with URL '";
                                    output_details+=links[i];
                                    output_details+="' was not found in the ZIM file.";
                                }
                                test_=false;
                            }
                            else
                                ct++;
                        }
                    }
                }
                progress.report();
            }
            if(test_)
                std::cout<<"\nPass\n";
            else
            {
                std::cout<<"\nFail\n";
                if(error_details)
                {
                    std::cout<<"Details: "<<output_details<<" "<<ct;
                }
            }
        }


        //Test 7: Checking for external Dependencies.
        if(run_all||url_check_external||no_args)
        {
            int found_count=0;
            std::cout<<"\nTest 7: Searching for External Dependencies: \n"<<std::flush;
            progress.initialise('#',c);
            test_=true;
            for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
            {
                if(it->getMimeType()=="text/html")
                {
                    std::vector<std::string> links=get_links(it->getPage());
                    for(int i=0; i<links.size(); i++)
                    {
                        if(!is_internal_url(links[i]))
                        {
                            if(!is_external_wikipedia_url(links[i]))
                            {
                                test_=false;
                            }
                        }
                    }
                }
                progress.report();
            }
            if(test_)
                std::cout<<"\nPass\n";
            else
            {
                std::cout<<"\nFail\n";
                if(error_details)
                {
                    std::cout<<"External Dependencies found in Articles\n";
                }
            }
        }

        //Test 8: Verifying MIME Types
        /*
                if(run_all||mime_check||no_args)
                {
                    std::cout<<"\nTest 8: Verifying MIME Types.. \n"<<std::flush;
                    progress.initialise('#',c);
                    test_=true;
                    for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
                    {
                        ;
                        progress.report();
                    }
                    if(test_)
                        std::cout<<"\nPass\n";
                    else
                    {
                        std::cout<<"\nFail\n";
                    }
                }
        */


    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

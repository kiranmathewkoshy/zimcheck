/*
 * Copyright (C)  Kiran Mathew Koshy
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
#include <unistd.h>
#include <zim/file.h>
#include <getopt.h>
#include <zim/fileiterator.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <regex>
#include <ctime>

std::vector <std::string> get_links2(std::string page)           //Returns a vector of the links in a particular page. includes links under 'href' and 'src'
{
    std::vector <std::string> links;
    int sz=page.size();
    for(int i=0; i<sz; i++)
    {
        if(page[i]==' '&&i+5<sz)
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

void get_links(std::string page, std::vector <std::string> *links)           //Returns a vector of the links in a particular page. includes links under 'href' and 'src'
{
    int sz=page.size();
    links->clear();
    int startingPoint,length;
    for(int i=0; i<sz; i++)
    {
        if(page[i]==' '&&i+5<sz)
        {
            if(page[i+1]=='h'&&page[i+2]=='r'&&page[i+3]=='e'&&page[i+4]=='f')      //Links under 'href' category.
            {
                i+=5;
                while(page[i]!='=')
                    i++;
                while(page[i]!='"')
                    i++;
                startingPoint= ++i;
                while(page[i]!='"')
                {
                    i++;
                }
                length=i-startingPoint;
                links->push_back(page.substr(startingPoint,length));
            }

            if(page[i+1]=='s'&&page[i+2]=='r'&&page[i+3]=='c')      //Links under 'src' category.
            {
                i+=4;
                while(page[i]!='=')
                    i++;
                while(page[i]!='"')
                    i++;
                startingPoint= ++i;

                while(page[i]!='"')
                {
                    i++;
                }
                length=i-startingPoint;
                links->push_back(page.substr(startingPoint,length));
            }
        }
    }
}


class progress_bar                                  //Class for implementing a progress bar(used in redundancy, url and MIME checks).
{
private:
    char icon;      //Character to be displayed
    int max_icons;  //maximum no. of characters to be displayed.
    int max_no;     //Maximum no of times report() will be called.
    int no;         //number of characters displayed(at a particular time). Similar to the counter variable, except it counts the number of times the character is displayed.
    int counter;    //Number of times report() has been called(at a particular time).
    bool is_initialised;//Boolean value to store wether the object has been initialised or not. report() will not display any characters
                        //if the is_initialised value is false.
    public:
    progress_bar(char icon_,int max_n)
    {
        if(max_n<1)
        {
            is_initialised=false;
            return;
        }
        if(max_n<1)
        {
            is_initialised=false;
            return;
        }
        is_initialised=true;
        max_icons=80;
        icon=icon_;
        max_no=max_n;
        no=0;
        counter=0;
        return;
    }
    void report()
    {
        if(!is_initialised)
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
            is_initialised=false;
        return;
    }
    void initialise(char icon_,int max_n)
    {
        if(max_n<1)
        {
            is_initialised=false;
            return;
        }
        is_initialised=true;
        max_icons=80;
        icon=icon_;
        max_no=max_n;
        no=0;
        counter=0;
        return;
    }
    void initialise(char icon_,int max_n,int max_ic)
    {
        if(max_n<1)
        {
            is_initialised=false;
            return;
        }
        if(max_ic<1)
        {
            is_initialised=false;
            return;
        }
        max_icons=max_ic;
        is_initialised=true;
        icon=icon_;
        max_no=max_n;
        no=0;
        counter=0;
        return;
    }
};

int adler32(std::string buf)                        //Adler32 Hash Function. Used to hash the BLOB data obtained from each article, for redundancy checks.
{
    unsigned int s1 = 1;
    unsigned int s2 = 0;
    int sz=buf.size();
    for (size_t n = 0; n <sz; n++)
    {
        s1 = (s1 + buf[n]) % 65521;
        s2 = (s2 + s1) % 65521;
    }
    return (s2 << 16) | s1;
}

bool is_external_url(std::string *s)       //Checks if an external URL is a wikipedia URL.
{
    if(std::regex_match( *s,std::regex(".*.wikipedia.org/.*")))
        return false;

    if(std::regex_match(*s,std::regex("/./.*")))
        return false;

    if(std::regex_match( *s,std::regex(".*.wikimedia.org/.*")))
        return  false;
    return true;
}

bool is_internal_url(std::string *s)                 //Checks if a URL is an internal URL or not. Uses RegExp.
{
    if(std::regex_match(*s,std::regex("/./.*")))
        return true;
    else
        return false;
}

//Removes extra spaces from URLs. Usually done by the browser, so web authors sometimes tend to ignore it.
//Converts the %20 to space.Essential for comparing URLs.
std::string process_links(std::string input)
{
    std::string output;
    output.clear();
    int pos=0;

    //URL Decoding.
    char ch;
    int i, ii;
    for (i=0; i<input.size(); i++)
    {
        if (int(input[i])==37)
        {
            sscanf(input.substr(i+1,2).c_str(), "%x", &ii);
            ch=static_cast<char>(ii);
            output+=ch;
            i=i+2;
        }
        else
        {
            output+=input[i];
        }
    }
    int k=output.rfind("#");
    return output.substr(0,k);
}

void display_help()
{
       std::cout<<"\n"
        "zimcheck\n"
        "Written by : Kiran Mathew Koshy\n"
    "(kiranmathewkoshy@gmail.com)\n"


    "This is a tool which can be used to check the quality of a ZIM file\n."
    "Here's a list of the checks that  are done on a ZIM file:\n"
    "1 - Internal checksum: launch internal checksum verification\n"
    "2 - Metadata Entries: checks if all metadata entries are present in the ZIM file.\n"
    "3 - Favicon: Checks if the favicon is present in the ZIM file.\n"
    "4 - Main Page entry: Checks wether the main page is present, and that it points to a valid article.\n"
    "5 - Redundant data check: Checks if there are any redundant articles.\n"
    "6 - Internal URL check: checks all interla URLs to make sure that they are valid URLs.\n"
    "7 - Searching for External Dependencies: The ZIM file is searched for external dependencies.\n"
    "8 - MIME type check: checks the validity of MIME type of all articles in the ZIM file.\n"
    "To list the details of the error reported, add a flag -D.\n"
    "For most errors, the details of the error will be quite lengthy, so it is reccomended that the user redirect the output to a file.\n"
    "Usage:\n"
    "./zimcheckusage: ./zimcheck [options] zimfile\n"
    "options:\n"
    "-A , --all             run all tests. Default if no flags are given.\n"
    "-C , --checksum        Internal CheckSum Test\n"
    "-M , --metadata        MetaData Entries\n"
    "-F , --favicon         Favicon\n"
    "-P , --main            Main page\n"
    "-R , --redundant       Redundant data check\n"
    "-U , --url_internal    URL check - Internal URLs\n"
    "-X , --url_external    URL check - Internal URLs\n"
    "-E , --mime            MIME checks\n"
    "-D , --details         Details of error\n"
    "-H , --help            Displays Help\n"
    "examples:\n"
    "./zimcheck -A wikipedia.zim\n"
  "./zimcheck --checksum --redundant wikipedia.zim\n"
  "./zimcheck -F -R wikipedia.zim\n"
  "./zimcheck -M --favicon wikipedia.zim\n"
  "./zimcheck -U wikipedia.zim\n"
  "./zimcheck -redundant -X wikipedia.zim\n"
  "./zimcheck -R -U -M wikipedia.zim\n";
    return;
}

int main (int argc, char **argv)
{

    bool overall_status=true;       //Overall status of test- wether it is a fail or a pass.

    //To calculate the total time taken by the program to run.
    time_t startTime,endTime;
    double  timeDiffference;
    time(&startTime);

    //The boolean values which will be used to store the output from getopt_long().
    //These boolean values will be then read by the program to execute the different parts of the program.

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
    bool help=false;
    int aflag = 0;
    int bflag = 0;
    int index;
    int c;
    std::string filename="";
    progress_bar progress('#',10);
    opterr = 0;

    //Parsing through arguments using getopt_long(). Both long and short arguments are allowed.
    while (1)
    {
        static struct option long_options[] =
             {
               {"all",     no_argument,       0, 'A'},
               {"checksum",  no_argument,       0, 'C'},
               {"metadata",  no_argument,       0, 'M'},
               {"favicon",  no_argument,       0, 'F'},
               {"main",  no_argument,       0, 'P'},
               {"redundant",  no_argument,       0, 'R'},
               {"url_internal",  no_argument,       0, 'U'},
               {"url_external",  no_argument,       0, 'X'},
               {"mime",  no_argument,       0, 'E'},
               {"details",  no_argument,       0, 'D'},
               {"help",  no_argument,       0, 'H'},
               {0, 0, 0, 0}
             };
        int option_index = 0;
        c = getopt_long (argc, argv, "ACMFPRUXEDHacmfpruxedh",
                            long_options, &option_index);
        //c = getopt (argc, argv, "ACMFPRUXED");
        if(c==-1)
            break;
        switch (c)
        {
        case 'A':
            run_all = true;
            break;
        case 'a':
            run_all = true;
            break;
        case 'C':
            checksum = true;
            break;
        case 'c':
            checksum = true;
            break;
        case 'M':
            metadata = true;
            break;
        case 'm':
            metadata = true;
            break;
        case 'F':
            favicon = true;
            break;
        case 'f':
            favicon = true;
            break;
        case 'P':
            main_page = true;
            break;
        case 'p':
            main_page = true;
            break;
        case 'R':
            redundant_data = true;
            break;
        case 'r':
            redundant_data = true;
            break;
        case 'U':
            url_check = true;
            break;
        case 'u':
            url_check = true;
            break;
        case 'X':
            url_check_external = true;
            break;
        case 'x':
            url_check_external = true;
            break;
        case 'E':
            mime_check = true;
            break;
        case 'e':
            mime_check = true;
            break;
        case 'D':
            error_details = true;
            break;
        case 'd':
            error_details = true;
            break;
        case 'H':
            help=true;
            break;
        case 'h':
            help=true;
            break;
        case '?':
            if (optopt == 'c')
                std::cerr<<"Option "<<(char)optopt<<" requires an argument.\n"
                         "options:\n"
                         "  -A , --all        run all tests. Default if no flags are given.\n"
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
    }

    //Displaying Help for --help argument
    if(help)
    {
        display_help();
        return -1;
    }

    //If no arguments are given to the program, all the tests are performed.
    if((run_all||checksum||metadata||favicon||main_page||redundant_data||url_check||url_check_external||mime_check||help)==false)
        no_args=true;

    //Obtaining filename from argument list
    filename="";
    for(int i=0;i<argc;i++)
    {
        if(argv[i][0]!='-'&&i!=0)
        {
            filename=argv[i];
        }
    }
    if(filename=="")
    {
        std::cerr<<"No file provided as argument\n";
        return -1;
    }
    //Tests.
    try
    {
        bool test_=false;
        zim::File f(filename);
        std::cout<<"\nRunning Tests...";
        int c=f.getFileheader().getArticleCount();
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
            overall_status&=test_;
        }

        //Test 2: Metadata Entries:
        //The file is searched for the compulsory metadata entries.
        if(run_all||metadata||no_args)
        {
            std::cout<<"\nTest  2: Metadata Entries: ";
            bool test_meta[6];
            for(int i=0; i<6; i++)
                test_meta[i]=false;
            for (zim::File::const_iterator it = f.findx('M', "").second; it != f.end() && it->getNamespace() == 'M'; ++it)
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
            overall_status&=test_;
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
            overall_status&=test_;
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
            overall_status&=test_;
        }


        //Test 5: Redundant Data:
        //The entire file is parsed, and the articles are hashed and stores in separate linked lists(std::list), one each for each namespace.
        //The lists are then sorted according to the hashes, and the articles with the same hashes are compared.
        //A list of pairs of indexes of articles(those that have the same hash) are then created.
        //Once the list of articles are created, they are compared one by one to see if they have the same content.If they do, it is reported to the user.
        if(run_all||redundant_data||no_args)
        {
            std::cout<<"\nTest 5: Redundant data: \n"<<std::flush;

            test_=false;
            int max=0;
            int k;
            progress.initialise('#',c,16);
            for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
            {
                k=it->getArticleSize();
                if(k>max)
                    max=k;
                progress.report();
            }
            //Data Storage system

            //The hashes are stored as a pair: The first element in the pair- unsigned int contains the hash obtained from the adler32 function.
            //The second element, an int, contains the index of the article in the ZIM file.
            //   std::pair<unsigned int, int>

            std::vector<std::list<std::pair<unsigned int, int> > >hash_main;

            //Allocating Double Hash Tree.
            hash_main.resize(max+1);

            //List of Articles to be compared against
            std::pair<unsigned int, int > article;

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
                article.first = adler32(ar);
                article.second=i;
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
            std::vector<std::pair<int,int> > to_verify;     //Vector containing list of pairs of articles whose hash has been found to be the same.
            //The above list of pairs of articles will be compared directly for redundancies.



            progress.initialise('#',hash_main_size,16);
            for(int i=0; i<hash_main_size; i++)
            {
                std::list<std::pair<unsigned int, int> >::iterator it=hash_main[i].begin();
                std::pair<unsigned int, int> prev;
                prev.second=it->second;
                prev.first=it->first;
                ++it;
                for (; it != hash_main[i].end(); ++it)
                {
                    if(it->first==prev.first)
                    {
                        if(it->second!=prev.second)
                        {
                            to_verify.push_back(std::make_pair(it->second,prev.second));
                        }
                    }
                    prev.second=it->second;
                    prev.first=it->first;
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
                for(int k=0; k<to_verify[i].first; k++)
                    ++it;
                s1=it->getPage();
                it = f.begin();
                for(int k=0; k<to_verify[i].second; k++)
                    ++it;
                s2=it->getPage();
                if(s1==s2)
                {
                    test_=false;
                    output_details+="\nArticles ";
                    output_details+=std::to_string(to_verify[i].first);
                    output_details+=" and ";
                    output_details+=std::to_string(to_verify[i].second);
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
            overall_status&=test_;
        }


        //Test 6: Verifying Internal URLs
        //All internal URLS are parsed, and compared with the existing articles in the zim file.
        //If the internal URL is not valid, an error is reported.
        //A list of Titles are collected from the file, and stored as a hash, similar to the way the hash is stored for redundancy check.
        //Each URL obtained is compared with the hash.
        if(run_all||url_check||no_args)
        {
            std::cout<<"\nTest 6: Verifying Internal URLs : \n"<<std::flush;
            std::vector < std::vector<std::string> >titles;
            titles.resize(256);
            std::string ar;
            for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
            {
                titles[(int)it->getNamespace()].push_back(it->getTitle());
            }
            for(int i=0; i<256; i++)
            {
                std::sort(titles[i].begin(),titles[i].end());
            }
            progress.initialise('#',c);
            test_=true;
            std::string output_details;
            std::string previousLink;
            int previousIndex=-1;
            int index;
            int k=0;
            std::vector<std::string> links;
            for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
            {
                if(it->getMimeType()=="text/html")
                {
                    get_links(it->getPage(),&links);
                    for(int i=0;i<links.size();i++)
                    {
                        //std::cout<<"\n"<<links[i]<<std::flush;
                        links[i]=process_links(links[i]);
                        //std::cout<<"\n"<<links[i]<<std::flush;
                        if(is_internal_url(&links[i]))
                        {   k++;
                            bool found=false;
                            int nm=(int)(links[i])[1];
                            if(std::binary_search(titles[nm].begin(),titles[nm].end(),(links[i]).substr(3)))
                                found=true;
                            if(!found)
                            {
                                if(error_details)
                                {
                                    index=it->getIndex();
                                    if((previousLink!=links[i])&&(previousIndex!=index))
                                    {
                                        output_details+="\nArticle '";
                                        output_details+=links[i];
                                        output_details+="' was not found. Linked in Article ";
                                        output_details+=std::to_string(index);
                                        previousLink=links[i];
                                        previousIndex=index;
                                    }
                                }
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
                    std::cout<<"Details: "<<output_details<<" ";
                }
            }
            overall_status&=test_;
        }


        //Test 7: Checking for external Dependencies.
        //All external URLs are parsed, and non-wikipedia URLs are reported.
        if(run_all||url_check_external||no_args)
        {
            int found_count=0;
            std::cout<<"\nTest 7: Searching for External Dependencies: \n"<<std::flush;
            progress.initialise('#',c);
            std::list<std::string> externalDependencyList;
            test_=true;
            std::vector<std::string> links;
            for (zim::File::const_iterator it = f.begin(); it != f.end(); ++it)
            {
                if(it->getMimeType()=="text/html")
                {
                    get_links(it->getPage(),&links);
                    for(int i=0;i<links.size();i++)
                    {
                        //if(!is_internal_url(&links[i]))
                        //{
                            if(is_external_url(&links[i]))
                            {
                                test_=false;
                                externalDependencyList.push_back(it->getUrl());
                            }
                        //}
                    }
                }
                progress.report();
            }
            if(test_)
                std::cout<<"\nPass\n"<<std::flush;
            else
            {
                std::cout<<"\nFail\n"<<std::flush;
                if(error_details)
                {
                    externalDependencyList.sort();
                    std::cout<<"External Dependencies found in the following Articles:\n"<<std::flush;
                    std::list<std::string>::iterator prev=externalDependencyList.begin();
                    std::cout<<"\n"<<*prev;
                    for(std::list<std::string>::iterator i=(++prev);i!=externalDependencyList.end();++i)
                    {
                        if(*i!=*prev)
                            std::cout<<"\n"<<*i<<std::flush;
                        prev=i;
                    }
                }
            }
            std::cout<<"\n"<<externalDependencyList.size()<<"\n"<<std::flush;
            overall_status&=test_;
        }

        //Test 8: Verifying MIME Types
        //MIME Checks is intended to verify that all the MIME types of all different articles are listed in the file header.
        //As of now, there is no method in the existing zimlib to get the list of MIME types listed in the file header.
        //A bug has been reported for the above problem, and once the bug is fixed, it will be used to add MIME checks to the zimcheck tool.
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
        std::cout<<"\nOverall Test Status: "<<std::flush;
        if(overall_status)
            std::cout<<"Pass\n"<<std::flush;
        else
            std::cout<<"Fail\n"<<std::flush;
        time(&endTime);

        timeDiffference=difftime(endTime,startTime);
        std::cout<<"\nTotal time taken: "<<timeDiffference<<" seconds.\n"<<std::flush;

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}







#include <bits/stdc++.h>
#include "hashlib/hl_hashwrapper.h"
#include "hashlib/hl_sha1wrapper.h"
#include "hashlib/hl_sha1wrapper.cpp"
#include "hashlib/hl_sha1.cpp"
#include "hashlib/hl_exception.h"
#include <io.h>

using namespace std;

set<string> files; 

string HEAD;

string getFileHash(string filename){
    hashwrapper *hasher = new sha1wrapper();
    try{
        return hasher->getHashFromFile(filename);
    }
    catch(hlException &e){
        cout << e.error_message();
    }
}

string getStringHash(string content){
    hashwrapper *hasher = new sha1wrapper();
    try{
        return hasher->getHashFromString(content);
    }
    catch(hlException &e){
        cout << e.error_message();
    }
}

class COMMIT{
    public:
        string hash;
        string message;
        string parent_hash;
        set<string> file_hash;
        COMMIT(){}
        COMMIT(string message, set<string> files, string parent_hash){
            this->message = message;
            this->parent_hash = parent_hash;
            this->hash = getStringHash(message + parent_hash);
            fstream fio;
            for(auto file: files){
                string hash = getFileHash(file);
                string filename = ".slit/objects/" + hash;
                this->file_hash.insert(filename);
                fio.open(filename,ios::in);
                if(fio.good()){
                    fio.close();
                    continue;
                }
                fio.close();

                fio.open(filename, ios::out);
                if(!fio) cout << "Error opening file";
                fio << file << endl;
                ifstream ifs(file);
                string line;
                while(!ifs.eof()){
                    getline(ifs, line);
                    fio << line << endl ;
                }
                fio.close();
            }
            
            HEAD = this->hash;
            fio.open(".slit/HEAD", ios::out);
            if(!fio) cout << "Error opening HEAD";
            fio << HEAD;
            fio.close();

            fio.open(".slit/index",ios::out | ios::trunc);
            fio.close();
        }
};

int init(){
    char basedir[] = ".slit";
    mkdir(basedir);
    
    char objdir[] = ".slit/objects";
    mkdir(objdir);

    char comdir[] = ".slit/commits";
    mkdir(comdir);

    fstream fio;
    fio.open(".slit/HEAD",ios::out);
    if(!fio) cout << "Error opening file";

    fio << "040f2cd867e49e6d0ab91a592799c5a9b0059540";
    fio.close();

    fio.open(".slit/index",ios::out);
    fio.close();
    return 0;
}

int docs(){
    cout << "Documentation goes here";
    return -1;
}

int commit(string message){
    COMMIT com(message, files, HEAD);
    ofstream ofs(".slit/commits/"+ com.hash, ios::app);

    if(!ofs) cout <<  "Error writing to commit file";
    cout << com.hash << endl;
    cout << com.message << endl;
    cout << com.parent_hash << endl;
    ofs.write((char*)&com, sizeof(com));
    ofs.close();
}

int add(string filename){
    fstream fio;
    fio.open(filename,ios::in);
    if(!fio.good()){
        cout << "File doesnt exist.";
        return -1;
    }
    fio.close();
    
    files.insert(filename);
    fio.open(".slit/index",ios::out);
    for (auto& file : files) fio << file << endl;
    fio.close();
}

int remove(string filename){
    fstream fio;
    files.erase(filename);
    fio.open(".slit/index",ios::out);
    for (auto& file : files) fio << file << endl;
    fio.close();
}



int main(int argc, char **argv){
    // if(argc == 0) return docs();
    // string command = argv[1];
    // if(command == "help") return docs();
    // if(command  == "init") return init();

    // ifstream ifs(".slit/index");
    // string file;
    // while(!ifs.eof()){
    //     getline(ifs, file);
    //     if(file == "") continue;
    //     files.insert(file);
    // }
    // ifs.close();
    // ifs.open(".slit/HEAD");
    // getline(ifs, HEAD);
    // ifs.close();

    // if(command == "commit") return commit(argv[2]);
    // if(command == "add") return add(argv[2]);
    // return 0;

    ifstream ifs(".slit/commits/e32ae588ad9729dfaf303577fb4fd7736f090bd0",ios::in);
    ifs.seekg(0);
    COMMIT ret;
    ifs.read((char *)&ret, sizeof(ret));
    ifs.close();
    cout << ret.message;
}


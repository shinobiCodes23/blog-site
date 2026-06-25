#include <bits/stdc++.h>
#include <fstream>
#include <filesystem>
#include  <crow.h>
using namespace std;

class Utils {
    public:
        string stringPreprocess(string s) {
            s.erase(remove(s.begin(), s.end(), ' '), s.end());
            for (int i=0; i<s.size(); i++) {
                if (isupper(s[i])) {
                    s[i]= (char)tolower(s[i]);
                }
            }
            return s;
        }  
    };

class Functions {
    private:
        Utils utils;
        vector<string> blogIndex; 
        unordered_map<string, vector<int>> keywords;
        unordered_map<string, int> fileIndex;
        string folderPath= getenv("FOLDER_PATH");
    public:
        Functions() {
            string l;
            if (!filesystem::exists(folderPath)) {
                filesystem::create_directories(folderPath);
            }
            for (const auto& entry : filesystem::directory_iterator(folderPath)) {
                if (entry.is_regular_file()) {
                    blogIndex.push_back(entry.path().filename());
                    fileIndex[utils.stringPreprocess(entry.path().filename())]= blogIndex.size()-1;
                    ifstream fin(folderPath+blogIndex[blogIndex.size()-1], ios::in);
                    getline(fin, l);
                    keywords[utils.stringPreprocess(l)].push_back(blogIndex.size()-1);
                    fin.close();
                }
            }
        }  
        vector<string> render() {
            return blogIndex;
        }
        vector<string> broadSearch(string s) {
            vector<string> res;
            s= utils.stringPreprocess(s);
            if (keywords.find(s)!=keywords.end()) {
                for (int i=0; i<keywords[s].size(); i++) {
                    res.push_back(blogIndex[keywords[s][i]]);
                }
            }
            else {
                res= {};
            }
            return res;
        }
        string narrowSearch(string s) {
            string res;
            s= utils.stringPreprocess(s);
            if (fileIndex.find(s)!=fileIndex.end()) {
                res= blogIndex[fileIndex[s]];
            }
            else {
                res= "";
            }
            return res;
        }
        bool addBlog(string fileName, string tag, string text) {
            bool res;
            ofstream fout(folderPath+fileName, ios::out);
            fout<<tag<<'\n';
            fout<<text;
            if (!fout.fail()) {
                res= true;
                keywords[utils.stringPreprocess(tag)].push_back(blogIndex.size());
                fileIndex[utils.stringPreprocess(fileName)]= blogIndex.size();
                blogIndex.push_back(fileName);
            }
            else {
                res= false;
            }
            fout.close();
            return res;
        }
        bool updateBlog(string fileName, string text) {
            bool res;
            ifstream fin(folderPath+fileName, ios::in);
            string tag;
            getline(fin, tag);
            fin.close();

            ofstream fout(folderPath+fileName, ios::out);
            fout<<tag<<'\n';
            fout<<text;
            if (!fout.fail()) {
                res= true;
            }
            else {
                res= false;
            }
            fout.close();
            return res;
        }
        string readBlog(string fileName) {
            ifstream fin(folderPath+fileName, ios::in);
            string line;
            string content;
            getline(fin, line);
            while (getline(fin, line)) {
                content+= line+'\n';
            }
            fin.close();
            return content;
        }
    };   

int main() {
    Functions obj;
    crow::SimpleApp app;
    crow::mustache::set_base("templates");
    CROW_ROUTE(app, "/")
    ([&obj](){
        auto page= crow::mustache::load("index.html");
        return page.render();    
    });
    CROW_ROUTE(app, "/add")
    ([&obj](){
        auto page= crow::mustache::load("add.html");
        return page.render();    
    });
    CROW_ROUTE(app, "/update")
    ([&obj](){
        auto page= crow::mustache::load("update.html");
        return page.render();    
    });
    CROW_ROUTE(app, "/renderBlogs")
    ([&obj] (const crow::request &req){
        vector<string> v= obj.render();
        crow::json::wvalue x;
        x["blogs"]= crow::json::wvalue::list();
        for (int i=v.size()-1; i>=0; i--) {
            x["blogs"][i]= v[i];
        }
        return x;
    });
    CROW_ROUTE(app, "/broadSearch")
    .methods("POST"_method)
    ([&obj](const crow::request &req) {
        auto body= crow::json::load(req.body);
        string s= body["keyword"].s();
        vector<string> v= obj.broadSearch(s);
        crow::json::wvalue x;
        x["blogs"]= crow::json::wvalue::list();
        for (int i=0; i<v.size(); i++) {
            x["blogs"][i]= v[i];
        }
        return x;
    });
    CROW_ROUTE(app, "/narrowSearch")
    .methods("POST"_method)
    ([&obj](const crow::request &req) {
        auto body= crow::json::load(req.body);
        crow::json::wvalue x;
        x["blogs"]= obj.narrowSearch(body["title"].s());
        return x;
    });
    CROW_ROUTE(app, "/addBlog")
    .methods("POST"_method)
    ([&obj](const crow::request &req) {
        auto body= crow::json::load(req.body);
        crow::json::wvalue x;
        x["bool"]= obj.addBlog(body["title"].s(), body["tag"].s(), body["text"].s());
        return x;
    });
    CROW_ROUTE(app, "/readBlog") 
    ([&obj](const crow::request &req){
        auto page= crow::mustache::load("blog.html");
        auto query= req.url_params.get("query");
        crow::json::wvalue x;
        x["title"]= query;
        x["content"]= obj.readBlog(query);
        return page.render(x);
    });
    CROW_ROUTE(app, "/updateBlog")
    .methods("POST"_method)
    ([&obj](const crow::request &req){
        auto body= crow::json::load(req.body);
        crow::json::wvalue x;
        x["bool"]= obj.updateBlog(body["title"].s(), body["text"].s());
        return x;
    });
    string Port_= getenv("PORT");
    int Port= (int) Port_;
    app.port(Port).multithreaded().run();
}
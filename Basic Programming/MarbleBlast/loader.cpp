#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

class Game_data
{
private:
    vector<string> m_names{"a"};
    vector<string> m_passwords{"a"};
    vector<int> m_highscores{1};
    vector<int>m_power{1};
    int m_current_user_index;
public:
    const vector<string>& getnames()
    {
        return m_names;
    }
    const vector<int>& gethighscores()
    {
        return m_highscores;
    }
    const vector<int>& getpowers()
    {
        return m_power;
    }
    Game_data()
    {
        loaduserdata();
    }
    bool add_user(string name,string password,int score,int power);     //add single user to vectors
    bool authenticate(string name,string password);                     //check user and pass of a name. if correct sets current user.
    void loaduserdata();                                                //read file and add all users
    void updatedata(int score,int power);                               //update score and power of the current user
    void savedata();                                                    //delete old file, rewrite the new one with new data
    void makeleader();

    void maw();                                                         //cout all data in vectors (DEBUG) :)
};
bool Game_data::add_user(string name,string password,int score,int power)
{
    if(find(m_names.begin(),m_names.end(),name)==m_names.end() and name!="" and password!="")
    {
        m_names.push_back(name);
        m_passwords.push_back(password);
        m_highscores.push_back(score);
        m_power.push_back(power);
        return true;
    }
    else
    {
        return false;
    }
}
void Game_data::loaduserdata()
{
    string line{},temp{},name{},password{};
    int score{},power{},comma1=0,comma2=0,mew=0;
    fstream file;
    file.open("data.txt");
//    getline(file,temp);
    cout<<"hi";
//    cout<<temp;
    while(!file.eof())
    {
        getline(file,line);
        comma2=line.find(",",comma1+1);
        while(comma2!=-1)
        {
            comma2=line.find(',',comma1+1);
//            cerr<<comma2;
            switch (mew)
            {
            case 0:
                name=line.substr(comma1,comma2-comma1);
//                cerr<<name<<'\n';
                break;
            case 1:
                password=line.substr(comma1+1,comma2-comma1-1);
//                cerr<<password<<'\n';
                break;
            case 2:
                score=stoi(line.substr(comma1+1,comma2-comma1));
//                cerr<<score<<'\n';
                break;
            case 3:
                power=stoi(line.substr(comma1+1,line.length()-comma1));
//                cerr<<power<<'\n';
                break;
            }
            ++mew;
            comma1=comma2;
        }
//        cerr<<"maybe here?";
        add_user(name,password,score,power);
//        cerr<<"maybe here?";
        mew=0;
        comma1=0;
//        cerr<<"maybe here?";
    }
//    cerr<<"come out?";
    file.close();
//    cerr<<"problem closing?";
}
bool Game_data::authenticate(string name,string password)
{
    int index = int(find(m_names.begin(),m_names.end(),name)-m_names.begin());
    if(index==int(m_names.end()-m_names.begin()))
        return false;
    else
    {
        if(m_passwords[index]==password)
        {
            m_current_user_index=index;
            return true;
        }
        else return false;
    }

}
void Game_data::updatedata(int score,int power)
{
    if(score>m_highscores[m_current_user_index])
    {
        m_highscores[m_current_user_index]=score;
        cerr<<"yeah";
    }
    m_power[m_current_user_index]=power;
}
void Game_data::savedata()
{
    fstream file;
    file.open("data.txt", std::ofstream::out | std::ofstream::trunc);
    int len=m_names.size();
    makeleader();
    for(int i=0; i<len; ++i)
        file<<m_names[i]<<','<<m_passwords[i]<<','<<m_highscores[i]<<','<<m_power[i]<<'\n';
    file.close();
}
void Game_data::maw()
{
    int len=m_names.size();
//    for(int i=0; i<len; ++i)
//        cout<<m_names[i]<<','<<m_passwords[i]<<','<<m_highscores[i]<<','<<m_power[i]<<'\n'<<"------------------------------\n";
}
void Game_data::makeleader()
{
    int index;
    for(vector<int>::iterator it=m_highscores.begin(); it!=m_highscores.end(); ++it)              //inefficient but does the job (O=n^2)
    {
        for(vector<int>::iterator jt=m_highscores.begin(); jt!=m_highscores.end()-1; ++jt)
        {
            if (*jt<*(jt+1))
            {
                index = int(jt-m_highscores.begin());
                swap(m_highscores[index],m_highscores[index+1]);
                swap(m_names[index],m_names[index+1]);
                swap(m_power[index],m_power[index+1]);
                swap(m_passwords[index],m_passwords[index+1]);
            }
        }
    }
}

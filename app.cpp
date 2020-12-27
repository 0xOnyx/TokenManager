
#include <string>
#include <fstream>
#include <limits>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <tuple>
#include <algorithm>
#include <sstream>
#include <array>
#include <functional>

enum class Commande {
  help,
  add,
  del,
  show,
  import,
  save,
  exit,
};

void viewHelp(){
  std::cout << "Avalaible command now => \n"
  << "0 - Help //is now \n"
  << "1 - add //add token format => Token | Name | 6 | base32 \n"
  << "2 - del //Del token \n"
  << "3 - show //Show token \n"
  << "4 - import //import token from file \n"
  << "5 - save //save token \n"
  << "10 - exit "
  << std::endl;
}

struct Info
{
  std::string site;
};

std::ostream & operator<<(std::ostream & flux , Info const & type){
  flux << type.site;
  return flux ;
};



struct TokenType {
  std::string nombre;
  std::string base;
};

std::ostream & operator<<(std::ostream & flux , TokenType const & type){
  flux << type.nombre << " | " << type.base;
  return flux ;
};



struct PassOtp {
  TokenType tokenType;
  Info info;
  std::string token;
};

std::ostream & operator<<(std::ostream & flux , PassOtp const & key){
  flux << key.token << " | " << key.info << " | " << key.tokenType;
  return flux;
};


void clean(std::string & texte){
  auto end { std::find_if_not(std::begin(texte), std::end(texte), isspace) };
  texte.erase(std::begin(texte), end);

  std::reverse(std::begin(texte), std::end(texte));

  end = std::find_if_not(std::begin(texte), std::end(texte), isspace);
  texte.erase(std::begin(texte), end);

  std::reverse(std::begin(texte), std::end(texte));
};



std::string separatorParse(std::istream & flux){

  std::ostringstream fullParams;

  std::string params {""};

  while(flux >> params && params != "|"){
    fullParams << params << " "; //add space between different words
  }

  return fullParams.str();
};



std::istream & operator>>(std::istream & flux , PassOtp & key){


  std::array<std::string*, 4> entryOptions {&key.token, &key.info.site, &key.tokenType.nombre, &key.tokenType.base};
  std::array<std::string, 4> entryOptionsDefaultValue {"TOKEN", "SITE", "6", "base32"};

  for(int i {0}; i < entryOptions.size(); i++ ){
    std::string params { separatorParse(flux) };

    if(std::empty(params)){
      params = entryOptionsDefaultValue[i];
    }

    clean(params);

    *entryOptions[i] = params;

  }

  return flux;
}



std::string entryUser(bool & exit){
  std::cout << ">";

  std::string commande {""};

  while( !( std::getline(std::cin, commande) ) )
  {
    if(std::cin.eof()){
      exit = true;
      throw std::runtime_error("your prompt is close");
    }
    else if(std::cin.fail()){
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      throw std::runtime_error("entry error");
    }
  }
  return commande;


};




std::tuple<Commande, std::string> commandeParse(std::string const inputUser)
{
  std::istringstream flux {inputUser};
  std::string commandeUser {""};
  flux >> commandeUser;
  clean(commandeUser);


  std::string instruction {""};
  std::getline(flux, instruction);
  clean(instruction);

  if(commandeUser == "exit" || commandeUser == "10"){
    return {Commande::exit, ""};
  }

  else if(commandeUser == "help" || commandeUser == "0"){
    return {Commande::help, instruction};
  }
  else if(commandeUser == "add" || commandeUser == "1"){
    return {Commande::add, instruction};
  }
  else if(commandeUser == "del" || commandeUser == "2"){
    return {Commande::del, instruction};
  }
  else if(commandeUser == "show" || commandeUser == "3"){
    return {Commande::show, instruction};
  }
  else if(commandeUser == "import" || commandeUser == "4"){
    return {Commande::import, instruction};
  }
  else if(commandeUser == "save" || commandeUser == "5"){
    return {Commande::save, instruction};
  }


  throw std::runtime_error("commande no found");

};

void saveToken(std::string const fileName, std::vector<PassOtp> const & libToken){
  std::ofstream file {fileName};
  if(!file){
    throw std::runtime_error("no file found !");
  }

  for(PassOtp token : libToken ){
    file << token << std::endl;
  }

}


void importToken(std::string const fileName, std::vector<PassOtp> & libToken)
{
  std::ifstream file {fileName};
  if(!file){
    throw std::runtime_error("no file found !");
  }

  std::string tokenLine {""};

  while(std::getline(file, tokenLine)){
    PassOtp token {};

    std::istringstream flux {tokenLine};

    flux >> token ;
    libToken.push_back(token);
  }
}

enum class ShowOptions {
  token,
  site,
  type,
  help,
};

std::string delimiteur {" --> "};

void showHelp(){
  std::cout << "Avalaible command for show => \n"
  << "0 - Help  \n"
  << "1 - token \n"
  << "2 - site \n"
  //<< "3 - type \n"
  << std::endl;
}



void showToken(std::vector<PassOtp> const & key){
  if(std::empty(key)){
    throw std::runtime_error("empty token");
  }
  for(int i {0}; i < key.size(); i++){
    std::cout << i+1 << delimiteur << key[i] << std::endl;
  }
}

void triBySite(std::vector<PassOtp> & key){
  auto triBysiteAnonyme = [](PassOtp keyA, PassOtp keyB)-> bool {
    return keyA.info.site == keyB.info.site;
  };
  std::sort(std::begin(key), std::end(key), triBysiteAnonyme );
}


void showSite(std::vector<PassOtp> & key) {
  if(std::empty(key)){
    throw std::runtime_error("empty token");
  }

  Info infoPrevious {};

  triBySite(key);
  for(int i {0}; i < key.size(); i++){
    if(key[i].info.site != infoPrevious.site){
      std::cout << '\n' ;
      std::cout << "=========" << key[i].info.site << "=========" << std::endl;
    }
    std::cout << '\t' << i+1 << delimiteur << key[i].token << key[i].tokenType << std::endl;
    infoPrevious = key[i].info;
  }
}

void show(ShowOptions options, std::vector<PassOtp> & key){
  switch(options){

    case ShowOptions::help :
      showHelp();
    break;

    case ShowOptions::token :
      showToken(key);
    break;

    case ShowOptions::site :
      showSite(key);
    break;

    case ShowOptions::type :
      //showType();
    break;

    default:
      showHelp();
  }
}

void delToken(std::string id, std::vector<PassOtp> & key){

  int numberToErase = std::stoi(id);

  if(numberToErase > key.size()){
    throw std::runtime_error("number greater than total => " + key.size());
  }

  else if(numberToErase <= 0 ){
    throw std::runtime_error("number smaller to 1");
  }

  key.erase(std::begin(key) + ( numberToErase - 1 ) );
}


void commandeExecute(Commande commande, std::string const params, std::vector<PassOtp> & libToken, bool & exit)
{
  switch(commande){

    case Commande::help :
        viewHelp();
    break;

    case Commande::exit :
      exit = true;
    break;

    case Commande::add :
    {
      std::istringstream flux {params};
      PassOtp token {};
      flux >> token;
      libToken.push_back(token);
    }
    break;

    case Commande::del :
      delToken(params, libToken);
    break;

    case Commande::show :
    {
      ShowOptions options {};
      if(params == "token" || params == "1"){
        options = ShowOptions::token;
      }
      else if(params == "site" || params == "2"){
        options = ShowOptions::site;
      }
      else if(params == "type" || params == "3"){
        options = ShowOptions::type;
      }
      else if(params == "help" || params == "0"){
        options = ShowOptions::help;
      }
      else{
        throw std::runtime_error("please show help for more informations");
      }
      show(options, libToken);
    }
    break;

    case Commande::import :
      importToken(params, libToken);
    break;

    case Commande::save :
      saveToken(params, libToken);
    break;

    default:
      viewHelp();
  }
};


int main(){
    //SHELL

    bool exit {false};

    std::vector<PassOtp> libToken {};

    do{

      try{
        std::string inputUser { entryUser(exit) };

        auto [ commande, params ] = commandeParse(inputUser);

        commandeExecute(commande, params, libToken, exit);

      }
      catch(std::runtime_error const & err){
        std::cout << "ERREUR 😵 => " << err.what() << std::endl;
      };

    }while(!exit);

    std::cout << "Bye 😉" << std::endl ;



    return 0;
}

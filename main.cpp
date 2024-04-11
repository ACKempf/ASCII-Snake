#include <iostream>
#include <vector>
#include <unistd.h>
#include <exception>
#include "TControl.hpp"

using namespace std;
//Define a type alias for vectors containing integer pairs called "p-vector", short for "pair vector"
using pvector = vector<pair<int, int>>;

int main()
{
  //Enable raw mode
  enableRawMode();

  //Input loop for confirming what display size the user wants
  //Also for taking dev commands
  while(true) {
  static char input;

  cout << "Adjust window to desired size and press any key to continue." << endl;
  cin.ignore();

  cout << "Enter y/Y to confirm display size: " << getTermSize().second << "x" << getTermSize().first << endl;
  cin >> input;
  if (input=='y'||input=='Y') break;
  cout << "\n\n\n";
  }

}
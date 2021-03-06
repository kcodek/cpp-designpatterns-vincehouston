﻿


// Purpose.  Command design pattern and class template

#include "CommonInclude.h"

class A {
   int divisor;
public:
   A( int div ) { divisor = div; }
   int divide( int in ) { return in / divisor; }
   int modulus( int in ) { return in % divisor; }
};

class B {
   string str;
public:
   B( string s ) { str = s; }
   string prepend(  string in ) { return in + str; }
   string postpend( string in ) { return str + in; }
};

template <typename CLS, typename ARG>
class Command {
public:
   typedef ARG (CLS::*Action)( ARG );
   Command( CLS* o, Action m, ARG a ) {
      object = o;
      method = m;
      argument = a;
   }
   ARG execute() { return (object->*method)( argument ); }
private:
   CLS*   object;
   Action method;
   ARG    argument;
};

int main_command_4A( void ) //TODO: taking the address of a temporary object of type 'Command<A, int>' [-Waddress-of-temporary]
{
   cout<<">>>>>>>> main_command_4A <<<<<<<<"<<endl;  
   Command<A,int>* list1[4] = {
      &Command<A,int>( &A(3), &A::divide,  16 ),
      &Command<A,int>( &A(3), &A::modulus, 16 ),
      &Command<A,int>( &A(4), &A::divide,  16 ),
      &Command<A,int>( &A(4), &A::modulus, 16 ) };
   for (int i=0; i < 4; i++)
      cout << "numbers are " << list1[i]->execute() << '\n';

   B obj("abc");
   Command<B,string>* list2[4] = {
      new Command<B,string>( &obj, &B::prepend,  "123" ),
      new Command<B,string>( &obj, &B::prepend,  "xyz" ),
      new Command<B,string>( &obj, &B::postpend, "123" ),
      new Command<B,string>( &obj, &B::postpend, "xyz" ) };
   for (int i=0; i < 4; i++)
      cout << "strings are " << list2[i]->execute() << '\n';
      return 0;
}

// numbers are 5
// numbers are 1
// numbers are 4
// numbers are 0
// strings are 123abc
// strings are xyzabc
// strings are abc123
// strings are abcxyz




// Purpose.  Command/Adapter design pattern (External Polymorphism demo)

// 1. Specify the new desired interface
// 2. Design a "wrapper" class that can "impedance match" the old to the new
// 3. The client uses (is coupled to) the new interface
// 4. The wrapper/adapter "maps" to the legacy implementation


class ExecuteInterface { public:                  // 1. Specify the new i/f
   virtual ~ExecuteInterface() { }
   virtual void execute() = 0;
};

template <class TYPE>                             // 2. Design a "wrapper" or
class ExecuteAdapter : public ExecuteInterface {  //    "adapter" class
public:
   ExecuteAdapter( TYPE* o, void (TYPE::*m)() ) { object = o;  method =m; }
   ~ExecuteAdapter()                            { delete object; }
   // 4. The adapter/wrapper "maps" the new to the legacy implementation
   void execute()             /* the new */     { (object->*method)(); }
private:                             
   TYPE* object;                                  // ptr-to-object attribute
   void (TYPE::*method)();    /* the old */       // ptr-to-member-function
};                                                //   attribute

// The old: three totally incompatible classes    // no common base class,
class Fea { public:                               // no hope of polymorphism
   ~Fea()        { cout << "Fea::dtor" << endl; }
   void doThis() { cout << "Fea::doThis()" << endl; }
};

class Feye { public:
   ~Feye()       { cout << "Feye::dtor" << endl; }
   void doThat() { cout << "Feye::doThat()" << endl; }
};

class Pheau { public:
   ~Pheau()          { cout << "Pheau::dtor" << endl; }
   void doTheOther() { cout << "Pheau::doTheOther()" << endl; }
};

/* the new is returned */ ExecuteInterface** initialize() {
   ExecuteInterface** array = new ExecuteInterface*[3]; /* the old is below */
   array[0] = new ExecuteAdapter<Fea>(   new Fea(),     &Fea::doThis       );
   array[1] = new ExecuteAdapter<Feye>(  new Feye(),    &Feye::doThat      );
   array[2] = new ExecuteAdapter<Pheau>( new Pheau(),   &Pheau::doTheOther );
   return array;
}

int main_command_4B( void ) 
{
      cout<<">>>>>>>> main_command_4B <<<<<<<<"<<endl;  
   ExecuteInterface** objects = initialize();

   for (int i=0; i < 3; i++) objects[i]->execute();  // 3. Client uses the new
                                                     //    (polymporphism)
   for (int i=0; i < 3; i++) delete objects[i];
   delete objects;
   return 0;
}

// Fea::doThis()
// Feye::doThat()
// Pheau::doTheOther()
// Fea::dtor
// Feye::dtor
// Pheau::dtor


// Purpose.  Command design pattern demo
// 
// Discussion.  Encapsulate a request as an object.  SimpleCommand
// maintains a binding between a receiver object and an action stored as a
// pointer to a member function.  MacroCommand maintains a sequence of
// Commands.  No explicit receiver is required because the subcommands
// already define their receiver.  MacroCommand may contain MacroCommands.



class Number {
public:
   Number( int value ) { _value = _copy = value; }
   int  getValue()     { return _value; }
   void increment()    { _copy = _value++; }
   void decrement()    { _copy = _value--; }
   void restore()      { _value = _copy; }
   void dubble()       { _copy = _value;  _value = 2 * _value; }
   void half()         { _copy = _value;  _value = _value / 2; }
   void square()       { _copy = _value;  _value = _value * _value; }
private:
   int _value;
   int _copy;
};


class Command2 {
public:
   virtual void execute() = 0;
   virtual void add(Command2*) { }
protected:
   Command2(){ }
};

class SimpleCommand : public Command2 {
public:
   typedef void (Number::* Action)();
   SimpleCommand( Number* receiver, Action action ) {
      _receiver = receiver;
      _action = action; }
   virtual void execute() { (_receiver->*_action)(); }
protected:
    Number* _receiver;
   Action  _action;
};

class MacroCommand : public Command2 {
public:
   MacroCommand() { _numCommands = 0; }
   void add( Command2* cmd ) { _list[_numCommands++] = cmd; }
   virtual void execute();
private:
   Command2* _list[10];
   int      _numCommands;
};

void MacroCommand::execute() {
   for (int i=0; i < _numCommands; i++)
      _list[i]->execute();
}

int main_command_4C() 
{
   cout<<">>>>>>>> main_command_4C <<<<<<<<"<<endl;  
   int i;
   cout << "Integer: ";
   cin >> i;
   Number*   object = new Number(i);

   Command2*  commands[9];
   commands[1] = new SimpleCommand( object, &Number::increment );
   commands[2] = new SimpleCommand( object, &Number::decrement );
   commands[3] = new SimpleCommand( object, &Number::dubble );
   commands[4] = new SimpleCommand( object, &Number::half );
   commands[5] = new SimpleCommand( object, &Number::square );
   commands[6] = new SimpleCommand( object, &Number::restore );
   commands[7] = new MacroCommand;
   commands[7]->add( commands[1] );
   commands[7]->add( commands[3] );
   commands[7]->add( commands[5] );
   commands[8] = new MacroCommand;
   commands[8]->add( commands[7] );
   commands[8]->add( commands[7] );

   cout << "Exit[0], ++[1], --[2], x2[3], Half[4], Square[5], "
      << "Undo[6], do3[7] do6[8]: ";
   cin >> i;

   while (i)
   {
      commands[i]->execute();
      cout << "   " << object->getValue() << endl;
      cout << "Exit[0], ++[1], --[2], x2[3], Half[4], Square[5], "
         << "Undo[6], do3[7] do6[8]: ";
      cin >> i;
   }
   return 0;
}

// Integer: 4
// Exit[0], ++[1], --[2], x2[3], Half[4], Square[5], Undo[6], do3[7] do6[8]: 1
//    5
// Exit[0], ++[1], --[2], x2[3], Half[4], Square[5], Undo[6], do3[7] do6[8]: 3
//    10
// Exit[0], ++[1], --[2], x2[3], Half[4], Square[5], Undo[6], do3[7] do6[8]: 2
//    9
// Exit[0], ++[1], --[2], x2[3], Half[4], Square[5], Undo[6], do3[7] do6[8]: 4
//    4
// Exit[0], ++[1], --[2], x2[3], Half[4], Square[5], Undo[6], do3[7] do6[8]: 5
//    16
// Exit[0], ++[1], --[2], x2[3], Half[4], Square[5], Undo[6], do3[7] do6[8]: 6
//    4
// Exit[0], ++[1], --[2], x2[3], Half[4], Square[5], Undo[6], do3[7] do6[8]: 6
//    4
// Exit[0], ++[1], --[2], x2[3], Half[4], Square[5], Undo[6], do3[7] do6[8]: 7
//    100
// Exit[0], ++[1], --[2], x2[3], Half[4], Square[5], Undo[6], do3[7] do6[8]: 6
//    10
// Exit[0], ++[1], --[2], x2[3], Half[4], Square[5], Undo[6], do3[7] do6[8]: 8
//    940900
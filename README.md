# SterlingExecutionEngine
C++/Qt based component demonstrating how to interface and place an order with the Sterling Trading Platform. 
The Sterling Trading platform  https://www.sterlingtradingtech.com/ does not support a C++ interface . 
I managed to generate a tlb from the distributed dll using OleView and interface to the library using COM.
The file ExecEngine.cpp contains the interface to the tlb library.Refer to the method SubscribeSterlingEvents to see how 
this component is subscribing to sterling events.The events are in SterlingEvnt.h

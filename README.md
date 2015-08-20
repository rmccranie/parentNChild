parentNChild

NOTE: I diverged from the problem statement as given in 2 ways:
  - I used C++ versus C.
  - I used C++-style file handling versus C-style.

This application implements the coding exercise described in the 
PDF file associated with this repository. The general idea is a
parent P creates children C1, C2….C10 each with file handles F1,F2….F10. 
Parent sends a message (say couple of random numbers), the child picks 
the numbers and computes the sum and writes them to their individual file.

The implementation for this example is admittedly contrived to demonstrate
comfort with a variety of topics. Among them C++ and associated object-
oriented concepts (probably most prominently good encapsulation technique),
use of open source libraries, and use of Unix/Linux interprocess 
communication (IPC) mechanisms.

The design chosen for the app is for parent and children to exist in separate 
processes but be single-threaded within them.  This exercise would have
been simple to implement if the design was based on a single-process, multi-
threaded model.  (The problem statement wording "use IPC" was take literally).

The IPC mechanism(s) chosen to facilitate this are a message queue for sending 
parent->child messages and signals to indicate completion.  Message queues
were chosen because I believe I can synchronize the children with parent
messaging (children "pend" on a queue) and because I wanted to play with them.

Building the application requires installation of the boost libraries (used
for creating the logging directory). "make" will build the app.  To run the app,
issue:
    "./parentNchild &" (put in background).  

To shutdown the app cleanly, issue:
   kill    -SIGHUP `pgrep Parent`   or
   killall -SIGHUP Parent

The app will log per the spec (though technically using streams rather 
than C style file handles). You may tail one of the log files to observe
the app in action. 

Items configurable from INI file:
  - Sending rate
  - Log path
  - Operation complexity


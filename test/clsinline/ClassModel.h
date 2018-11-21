#pragma once

/*
  Class union:

  class D is the dependent, class C is the host

  ---------------------
  Inlining codependents
  ---------------------

  (A)  C.foo points to D,  D.owner points to C
  
  Use: D is extending C so that it participates in 
  relationships. Example is ListNode, where inlining
  can be thought of as "intrusive data structures"
  
  Gain: speed, memory consumption
  Lose: program size, unless D can be erased

  Simple case:

  class D {
     final Object key;
     D prev;
     D next;
  }


  class C {


    +C prev;
    +C next;

  };


  ------------------
  Inlining resources
  ------------------

  (B) C.foo points to D , nothing from D points to C
  
  Use: D is some resource class, e.g. like PrintStream,
  where the identity of instances is not important.

  For example, we can then "inline" an object into a
  stack frame (avoiding malloc?)

  
  Gain: speed, memory consumption
  Lose: program size, unless D can be erased


  Simple case:
  class D {  // extends Object
    D() { ... }

    int a;
    String b;

    void bar() { ... }

  }

  class C {
    - D foo; 

    C() {
      

      - foo = new D();
      + // D() translated a->foo_a b->foo_b
        // as if C was a subclass of D
    }


    + int foo_a;
    + String foo_b;

    + void foo_bar() { .. }
  }



*/


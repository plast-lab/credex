package clsinline.trivial;

import clsinline.trivial.RsrcClass;

public class HostClass
{

  // inline this class here
  RsrcClass foo;

  public HostClass() {
    foo = new RsrcClass("hello ");
  }

  public String greeting(int n) {
	   return foo.addtimes(n) + "world";
  }
}

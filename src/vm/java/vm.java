import java.io.*;
import jaja.*;

public class vm {

  public static void main(String args[]) throws Exception {

    if (1 != args.length) {
      System.out.println("Usage: jajavm ModName");
      return;
    }

    jaja rt = new jaja();
    if (rt.invoke(args[0])) {
      while (rt.runStep()) {
      }
    }
  }
}

#include <stdio.h>

extern void parse(char *, int);

static void printHTMLHeader() {
  puts(
    "Content-Type: text/html\n"
    "\n"
    "<!DOCTYPE html>\n"
    "<html>\n"
    "\n"
    "<head>\n"
    "  <title>sysctl hw machdep vm</title>\n"
    "  <meta name=\"viewport\" content=\"width=device, initial-scale=1\" />\n"
    "  <link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n"
    "</head>\n"
    "\n"
    "<body>\n"
    "  <h2>sysctl hw machdep vm</h2>\n"
    "  <pre>"
  );
}

static void printHTMLFooter() {
  puts(
    "  </pre>\n"
    "</body>\n"
    "\n"
    "</html>"
  );
}

int main(int argc, char** argv) {
  printHTMLHeader();

  parse("hw", 1);
  parse("machdep", 1);
  parse("vm", 1);

  printHTMLFooter();
  
  return 0;
}

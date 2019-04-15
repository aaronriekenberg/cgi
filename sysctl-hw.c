/*
 * Mostly lifted from https://github.com/openbsd/src/blob/master/sbin/sysctl/sysctl.c
 */

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/sensors.h>
#include <sys/sysctl.h>

static struct ctlname topname[] = CTL_NAMES;
static struct ctlname hwname[] = CTL_HW_NAMES;

static void printStringCtlValue(
  size_t topLevelID,
  size_t secondLevelID,
  struct ctlname* ctlname) {
  int mib[2];
  size_t oldlenp;
  static char* stringValue = NULL;
  static size_t stringValueCapacity = 0;

  mib[0] = topLevelID;
  mib[1] = secondLevelID;

  oldlenp = 0;
  if (sysctl(mib, 2, NULL, &oldlenp, NULL, 0) == -1) {
    if (errno == EOPNOTSUPP) {
      return;
    } else {
      err(1, "sysctl %zu", secondLevelID);
    }
  }

  if ((stringValue == NULL) || (oldlenp > stringValueCapacity)) {
    stringValueCapacity = oldlenp;
    stringValue = reallocarray(stringValue, stringValueCapacity, sizeof(char));
  }
  if (sysctl(mib, 2, stringValue, &oldlenp, NULL, 0) == -1) {
    if (errno == EOPNOTSUPP) {
      return;
    } else {
      err(1, "sysctl %zu", secondLevelID);
    }
  }

  printf("%s.%s=%s\n", topname[topLevelID].ctl_name, ctlname->ctl_name, stringValue);
}

static void printIntCtlValue(
  size_t topLevelID,
  size_t secondLevelID,
  struct ctlname* ctlname) {
  int mib[2];
  size_t oldlenp;
  int intValue;

  mib[0] = topLevelID;
  mib[1] = secondLevelID;

  oldlenp = sizeof(intValue);
  if (sysctl(mib, 2, &intValue, &oldlenp, NULL, 0) == -1) {
    if (errno == EOPNOTSUPP) {
      return;
    } else {
      err(1, "sysctl %zu", secondLevelID);
    }
  }

  printf("%s.%s=%d\n", topname[topLevelID].ctl_name, ctlname->ctl_name, intValue);
}

static void printQuadCtlValue(
  size_t topLevelID,
  size_t secondLevelID,
  struct ctlname* ctlname) {
  int mib[2];
  size_t oldlenp;
  int64_t quadValue;

  mib[0] = topLevelID;
  mib[1] = secondLevelID;

  oldlenp = sizeof(quadValue);
  if (sysctl(mib, 2, &quadValue, &oldlenp, NULL, 0) == -1) {
    if (errno == EOPNOTSUPP) {
      return;
    } else {
      err(1, "sysctl %zu", secondLevelID);
    }
  }

  printf("%s.%s=%lld\n", topname[topLevelID].ctl_name, ctlname->ctl_name, quadValue);
}

static void printSensors(
  size_t topLevelID,
  size_t secondLevelID,
  struct ctlname* ctlname) {
  int mib[5];
  size_t dev;
  struct sensordev snsrdev;
  size_t sdlen;
  enum sensor_type type;
  int numt;

  mib[0] = topLevelID;
  mib[1] = secondLevelID;
  for (dev = 0; ; ++dev) {
    mib[2] = dev;  

    sdlen = sizeof(snsrdev);
    if (sysctl(mib, 3, &snsrdev, &sdlen, NULL, 0) == -1) {
      if (errno == ENXIO) {
	continue;
      }
      if (errno == ENOENT) {
        break;
      }
      err(1, "sensors dev %zu", dev);
    }

    for (type = 0; type < SENSOR_MAX_TYPES; ++type) {
      mib[3] = type;

      for (numt = 0; numt < snsrdev.maxnumt[type]; ++numt) {
        struct sensor snsr;
        size_t slen;

        mib[4] = numt;

        slen = sizeof(snsr);
        if (sysctl(mib, 5, &snsr, &slen, NULL, 0) == -1) {
          continue;
        }

        if ((slen > 0) &&
            ((snsr.flags & SENSOR_FINVALID) == 0) &&
            ((snsr.flags & SENSOR_FUNKNOWN) == 0)) {
          printf("%s.%s.%s.%s%d=",
                 topname[topLevelID].ctl_name, ctlname->ctl_name,
                 snsrdev.xname, sensor_type_s[type], numt);
          switch (snsr.type) {
          case SENSOR_TEMP:
            printf("%.2f degC\n", (snsr.value - 273150000) / 1000000.0);
            break;
          default:
            printf("unknown\n");
            break;
          }
        }
      }
    }
  }
}

static void printHardwareInfo() {
  size_t i;

  for (i = 0; i < HW_MAXID; ++i) {
    if (hwname[i].ctl_type == CTLTYPE_STRING) {
      printStringCtlValue(CTL_HW, i, hwname + i);
    } else if (hwname[i].ctl_type == CTLTYPE_INT) {
      printIntCtlValue(CTL_HW, i, hwname + i);
    } else if (hwname[i].ctl_type == CTLTYPE_QUAD) {
      printQuadCtlValue(CTL_HW, i, hwname + i);
    } else if ((hwname[i].ctl_type == CTLTYPE_NODE) &&
               (strcmp(hwname[i].ctl_name, "sensors") == 0)) {
      printSensors(CTL_HW, i, hwname + i);
    }
  }

}

static void printHTMLHeader() {
  puts(
    "Content-Type: text/html\n"
    "\n"
    "<!DOCTYPE html>\n"
    "<html>\n"
    "\n"
    "<head>\n"
    "  <title>sysctl hw</title>\n"
    "  <meta name=\"viewport\" content=\"width=device, initial-scale=1\" />\n"
    "  <link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n"
    "</head>\n"
    "\n"
    "<body>\n"
    "  <h2>sysctl hw</h2>\n"
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
  printHardwareInfo();
  printHTMLFooter();
  
  return 0;
}

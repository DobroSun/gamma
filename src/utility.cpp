#include "gamma/pch.h"
#include "gamma/utility.h"
#include "gamma/globals.h"

int numrows() {
  return (Height - TextUpperBound) / (ptsize+blines);
}
int totalrows() {
  return Height / (ptsize+blines);
}

// Change Serial prints

// Function to send a string over UART
void putString(const char *str) {
  // Loop through each character in the string
  while (*str != '\0') {
    putChar(*str);
    str++;
  }
}
// Function to send an integer over UART
void putInt(int num) {
  // Convert the integer to a string
  char buffer[10];
  itoa(num, buffer, 10);
  putString(buffer);
}

// Example
void RTC_fun() {
  putInt(RTC.getHours());
  putChar(':');
  putInt(RTC.getMinutes());
  putChar(':');
  putInt(RTC.getSeconds());
  putChar('\n');
}
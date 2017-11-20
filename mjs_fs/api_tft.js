let TFT = {
  // ## **`TFT.PrintDate(x, y, font)`**
  // Print current date to TFT
  //
  // Return: none.
  //
  // Example:
  // ```javascript
  // TFT.PrintDate(100, 20, 2);
  // ```
  // Note:
  // Check the font types from tft.h.
  PrintDate: ffi('void mgos_tft_print_date(int, int, int)'),
};

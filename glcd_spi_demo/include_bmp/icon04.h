// ---------------------------------------------
//             Bitmap Image aus BMP-Datei
// ---------------------------------------------
static const uint8_t bmpimage04[] = {
  0x00, 0x21, 0x00, 0x25,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x0D, 0x0D,
  0x0D, 0x06, 0x24, 0x25, 0x25, 0x27, 0x24, 0x25,
  0x24, 0x25, 0x25, 0x22, 0x25, 0x27, 0x25, 0x27,
  0x26, 0x26, 0x22, 0x24, 0x25, 0x2A, 0x07, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0x25, 0x9F,
  0x99, 0x98, 0x99, 0x98, 0x98, 0x85, 0x85, 0x8C,
  0x8C, 0x8C, 0x8C, 0x85, 0x8C, 0x8C, 0x8C, 0x85,
  0x52, 0x87, 0x87, 0x52, 0x52, 0x52, 0x51, 0x26,
  0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0x25,
  0x9B, 0x8F, 0x8E, 0x8E, 0x8E, 0x82, 0x81, 0x82,
  0x81, 0x82, 0x82, 0x81, 0x82, 0x81, 0x52, 0x52,
  0x89, 0x83, 0x52, 0x52, 0x52, 0x85, 0x52, 0x51,
  0x26, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1,
  0x25, 0x9B, 0x8F, 0x8E, 0x8E, 0x8E, 0x8E, 0x88,
  0x82, 0x82, 0x81, 0x81, 0x81, 0x81, 0x89, 0x81,
  0x82, 0x82, 0x89, 0x83, 0x8A, 0x8A, 0x52, 0x52,
  0x51, 0x24, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xEF, 0x25, 0x8F, 0x8E, 0x8E, 0x8E, 0x88,
  0x82, 0x82, 0x82, 0x81, 0x81, 0x81, 0x82, 0x81,
  0x89, 0x82, 0x52, 0x81, 0x52, 0x52, 0x52, 0x8A,
  0x52, 0x26, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0x25, 0x98, 0x8E, 0x8E, 0x8E,
  0x8E, 0x88, 0x82, 0x82, 0x81, 0x81, 0x89, 0x88,
  0x81, 0x89, 0x81, 0x89, 0x88, 0x81, 0x83, 0x52,
  0x52, 0x52, 0x25, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0x06, 0x99, 0x8F, 0x8F,
  0x8E, 0x8E, 0x82, 0x88, 0x83, 0x85, 0x8C, 0x52,
  0x52, 0x52, 0x52, 0x52, 0x8A, 0x88, 0x52, 0x52,
  0x52, 0x8C, 0x52, 0x24, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0x27, 0x99,
  0x8F, 0x8E, 0x8E, 0x88, 0x8A, 0x87, 0x32, 0x26,
  0x26, 0x26, 0x23, 0x32, 0x52, 0x85, 0x83, 0x83,
  0x83, 0x52, 0x52, 0x26, 0xEF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0x0D,
  0x9B, 0x99, 0x8F, 0x88, 0x8A, 0x85, 0x32, 0x96,
  0xB1, 0x74, 0x29, 0xA8, 0x9C, 0x32, 0x52, 0x85,
  0x52, 0x52, 0x87, 0x51, 0x24, 0xF1, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1,
  0x0D, 0x9F, 0x99, 0x98, 0x85, 0x85, 0x27, 0x99,
  0x26, 0x9C, 0x26, 0x26, 0xA9, 0x26, 0x52, 0x32,
  0x52, 0x87, 0x52, 0x52, 0x51, 0x25, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xEF, 0x0D, 0x0D, 0x9B, 0x99, 0x33, 0x98,
  0x87, 0x56, 0x26, 0x53, 0x52, 0x26, 0x56, 0x87,
  0x52, 0x32, 0x52, 0x53, 0x26, 0x25, 0xEF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xED, 0x0E, 0x0D, 0x27, 0x98,
  0x8C, 0x87, 0x32, 0x52, 0x85, 0x8C, 0x8C, 0x32,
  0x8C, 0x87, 0x52, 0x26, 0x26, 0x2B, 0xF1, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0xEF, 0x25, 0x99,
  0x98, 0x8C, 0x33, 0x52, 0x87, 0x8C, 0x85, 0x8C,
  0x8C, 0x33, 0x8C, 0x8C, 0x52, 0x22, 0xF1, 0xF1,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0x25, 0x27, 0x27,
  0x8A, 0x88, 0x33, 0x52, 0x56, 0x26, 0x32, 0x32,
  0x32, 0x56, 0x87, 0x33, 0x8A, 0x8C, 0x26, 0x26,
  0x25, 0xEF, 0xED, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0x25, 0x56, 0x56,
  0x8C, 0x88, 0x8A, 0x87, 0x56, 0x26, 0xA9, 0xA8,
  0xA8, 0xA9, 0x32, 0x92, 0x87, 0x88, 0x88, 0x8D,
  0x52, 0x51, 0x25, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x24, 0x9B, 0x98,
  0x85, 0x8A, 0x52, 0x85, 0x51, 0x26, 0xA9, 0xA8,
  0xAE, 0xAE, 0xA8, 0xA4, 0x26, 0x56, 0x8A, 0x81,
  0x52, 0x8C, 0x52, 0x51, 0x24, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x25, 0x99,
  0x8F, 0x8B, 0x52, 0x52, 0x52, 0x25, 0x29, 0xA8,
  0x32, 0x21, 0x32, 0x23, 0xB0, 0x2B, 0x24, 0x87,
  0x8A, 0x88, 0x83, 0x52, 0x52, 0x25, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0D,
  0x9B, 0x8F, 0x88, 0x8A, 0x52, 0x25, 0x78, 0x59,
  0x24, 0xA5, 0x95, 0x97, 0xA5, 0x22, 0x58, 0xD4,
  0x25, 0x56, 0x8A, 0x83, 0x52, 0x51, 0x25, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xEF, 0x25, 0x9B, 0x98, 0x87, 0x0D, 0x78, 0x59,
  0x0C, 0xB1, 0xA5, 0x35, 0x35, 0x97, 0xAC, 0x0C,
  0x59, 0x79, 0x0D, 0x87, 0x87, 0x52, 0x26, 0xEF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xF1, 0x0D, 0x0D, 0x0D, 0x6D, 0x59,
  0x0C, 0xA8, 0xA2, 0x97, 0x9D, 0x95, 0x97, 0xA5,
  0x74, 0x07, 0x5C, 0x79, 0x2A, 0x25, 0x06, 0xF1,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0x1A, 0x1A, 0x6A, 0x65, 0x6A, 0x6B,
  0x14, 0x74, 0x20, 0x45, 0xB5, 0x9D, 0x96, 0x21,
  0x45, 0xCB, 0x74, 0x14, 0x6F, 0x59, 0x7E, 0x7E,
  0x14, 0x1A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0x1B, 0x6B, 0x6E, 0x63, 0x6B, 0x59,
  0x59, 0x16, 0x74, 0x20, 0x45, 0xB9, 0xA5, 0xA5,
  0x20, 0x45, 0xCA, 0x74, 0x16, 0x59, 0x6B, 0x64,
  0x65, 0x6F, 0x6F, 0x14, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0x1A, 0x6B, 0x6E, 0x63, 0x6E, 0x59,
  0x16, 0x0A, 0x07, 0xAC, 0x04, 0x00, 0x04, 0xA4,
  0x9C, 0x20, 0x04, 0x00, 0x74, 0x0C, 0x16, 0x0A,
  0x59, 0x6F, 0x63, 0x6E, 0x7F, 0x11, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0x1A, 0x6B, 0x64, 0x6E, 0x5C,
  0x0C, 0x3A, 0x3A, 0x25, 0x32, 0xAC, 0xAE, 0xAE,
  0xA8, 0xB1, 0xA4, 0xA8, 0xA8, 0x26, 0x29, 0x3A,
  0x3A, 0x2A, 0x59, 0x6F, 0x59, 0x7E, 0x11, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1A, 0xF9, 0x7F,
  0x0A, 0x38, 0x33, 0x33, 0x22, 0x26, 0xA8, 0x32,
  0xAC, 0x23, 0x26, 0xB1, 0x26, 0xA8, 0x26, 0x26,
  0x33, 0x33, 0x38, 0x16, 0x6B, 0x7E, 0x10, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1A,
  0x59, 0x0C, 0x33, 0x33, 0x26, 0x57, 0x26, 0xB0,
  0x2B, 0x26, 0x33, 0x33, 0x25, 0x22, 0xA9, 0x26,
  0x56, 0x32, 0x33, 0x33, 0x0C, 0x59, 0x0A, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x16, 0x06, 0x33, 0x33, 0x23, 0xB4, 0x57,
  0x22, 0x25, 0x33, 0x9F, 0x92, 0x33, 0x26, 0x32,
  0x57, 0x92, 0x32, 0x33, 0x33, 0x07, 0x16, 0xFF,
  0xED, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0x2C, 0x33, 0x33, 0x33, 0x26,
  0x33, 0xB4, 0x56, 0x23, 0x35, 0x31, 0x32, 0x56,
  0x91, 0x35, 0x32, 0x33, 0x33, 0x38, 0x0C, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0x29, 0x33, 0x33,
  0x33, 0x32, 0x32, 0x26, 0x91, 0xA6, 0x31, 0x92,
  0x32, 0x32, 0x32, 0x33, 0x33, 0x3A, 0x29, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x2B,
  0x3A, 0x33, 0x33, 0x33, 0x9F, 0x32, 0x32, 0x32,
  0x32, 0x9F, 0x33, 0x33, 0x33, 0x3A, 0x2C, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x2C, 0x2B, 0x23, 0x33, 0x33, 0x33, 0x3A,
  0x33, 0x33, 0x33, 0x33, 0x26, 0x26, 0x29, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0x2A, 0x2C, 0x2C,
  0x2C, 0x07, 0x05, 0x2C, 0x2C, 0xF1, 0xEE, 0xF1,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/*
   Based on https://github.com/AaronLiddiment/LEDSprites/blob/master/examples/Tetris/Tetris.ino
   Author: https://github.com/AaronLiddiment/
*/
const uint8_t TetrisIData[] =
{
  // Frame 1
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(11110000),
  // Frame 2
  B8_3BIT(10000000),
  B8_3BIT(10000000),
  B8_3BIT(10000000),
  B8_3BIT(10000000),
  // Frame 3
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(11110000),
  // Frame 4
  B8_3BIT(10000000),
  B8_3BIT(10000000),
  B8_3BIT(10000000),
  B8_3BIT(10000000)
};
const uint8_t TetrisIMask[] =
{
  // Frame 1
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(11110000),
  // Frame 2
  B8_1BIT(10000000),
  B8_1BIT(10000000),
  B8_1BIT(10000000),
  B8_1BIT(10000000),
  // Frame 3
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(11110000),
  // Frame 4
  B8_1BIT(10000000),
  B8_1BIT(10000000),
  B8_1BIT(10000000),
  B8_1BIT(10000000)
};
const uint8_t TetrisJData[] =
{
  // Frame 1
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(22200000),
  B8_3BIT(00200000),
  // Frame 2
  B8_3BIT(00000000),
  B8_3BIT(02000000),
  B8_3BIT(02000000),
  B8_3BIT(22000000),
  // Frame 3
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(20000000),
  B8_3BIT(22200000),
  // Frame 4
  B8_3BIT(00000000),
  B8_3BIT(22000000),
  B8_3BIT(20000000),
  B8_3BIT(20000000)
};
const uint8_t TetrisJMask[] =
{
  // Frame 1
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(11100000),
  B8_1BIT(00100000),
  // Frame 2
  B8_1BIT(00000000),
  B8_1BIT(01000000),
  B8_1BIT(01000000),
  B8_1BIT(11000000),
  // Frame 3
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(10000000),
  B8_1BIT(11100000),
  // Frame 4
  B8_1BIT(00000000),
  B8_1BIT(11000000),
  B8_1BIT(10000000),
  B8_1BIT(10000000)
};
const uint8_t TetrisLData[] =
{
  // Frame 1
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(33300000),
  B8_3BIT(30000000),
  // Frame 2
  B8_3BIT(00000000),
  B8_3BIT(33000000),
  B8_3BIT(03000000),
  B8_3BIT(03000000),
  // Frame 3
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(00300000),
  B8_3BIT(33300000),
  // Frame 4
  B8_3BIT(00000000),
  B8_3BIT(30000000),
  B8_3BIT(30000000),
  B8_3BIT(33000000)
};
const uint8_t TetrisLMask[] =
{
  // Frame 1
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(11100000),
  B8_1BIT(10000000),
  // Frame 2
  B8_1BIT(00000000),
  B8_1BIT(11000000),
  B8_1BIT(01000000),
  B8_1BIT(01000000),
  // Frame 3
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(00100000),
  B8_1BIT(11100000),
  // Frame 4
  B8_1BIT(00000000),
  B8_1BIT(10000000),
  B8_1BIT(10000000),
  B8_1BIT(11000000)
};
const uint8_t TetrisOData[] =
{
  // Frame 1
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(44000000),
  B8_3BIT(44000000),
  // Frame 2
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(44000000),
  B8_3BIT(44000000),
  // Frame 3
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(44000000),
  B8_3BIT(44000000),
  // Frame 4
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(44000000),
  B8_3BIT(44000000)
};
const uint8_t TetrisOMask[] =
{
  // Frame 1
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(11000000),
  B8_1BIT(11000000),
  // Frame 2
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(11000000),
  B8_1BIT(11000000),
  // Frame 3
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(11000000),
  B8_1BIT(11000000),
  // Frame 4
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(11000000),
  B8_1BIT(11000000)
};
const uint8_t TetrisSData[] =
{
  // Frame 1
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(05500000),
  B8_3BIT(55000000),
  // Frame 2
  B8_3BIT(00000000),
  B8_3BIT(50000000),
  B8_3BIT(55000000),
  B8_3BIT(05000000),
  // Frame 3
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(05500000),
  B8_3BIT(55000000),
  // Frame 4
  B8_3BIT(00000000),
  B8_3BIT(50000000),
  B8_3BIT(55000000),
  B8_3BIT(05000000)
};
const uint8_t TetrisSMask[] =
{
  // Frame 1
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(01100000),
  B8_1BIT(11000000),
  // Frame 2
  B8_1BIT(00000000),
  B8_1BIT(10000000),
  B8_1BIT(11000000),
  B8_1BIT(01000000),
  // Frame 3
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(01100000),
  B8_1BIT(11000000),
  // Frame 4
  B8_1BIT(00000000),
  B8_1BIT(10000000),
  B8_1BIT(11000000),
  B8_1BIT(01000000)
};
const uint8_t TetrisTData[] =
{
  // Frame 1
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(66600000),
  B8_3BIT(06000000),
  // Frame 2
  B8_3BIT(00000000),
  B8_3BIT(06000000),
  B8_3BIT(66000000),
  B8_3BIT(06000000),
  // Frame 3
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(06000000),
  B8_3BIT(66600000),
  // Frame 4
  B8_3BIT(00000000),
  B8_3BIT(60000000),
  B8_3BIT(66000000),
  B8_3BIT(60000000)
};
const uint8_t TetrisTMask[] =
{
  // Frame 1
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(11100000),
  B8_1BIT(01000000),
  // Frame 2
  B8_1BIT(00000000),
  B8_1BIT(01000000),
  B8_1BIT(11000000),
  B8_1BIT(01000000),
  // Frame 3
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(01000000),
  B8_1BIT(11100000),
  // Frame 4
  B8_1BIT(00000000),
  B8_1BIT(10000000),
  B8_1BIT(11000000),
  B8_1BIT(10000000)
};
const uint8_t TetrisZData[] =
{
  // Frame 1
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(77000000),
  B8_3BIT(07700000),
  // Frame 2
  B8_3BIT(00000000),
  B8_3BIT(07000000),
  B8_3BIT(77000000),
  B8_3BIT(70000000),
  // Frame 3
  B8_3BIT(00000000),
  B8_3BIT(00000000),
  B8_3BIT(77000000),
  B8_3BIT(07700000),
  // Frame 4
  B8_3BIT(00000000),
  B8_3BIT(07000000),
  B8_3BIT(77000000),
  B8_3BIT(70000000)
};
const uint8_t TetrisZMask[] =
{
  // Frame 1
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(11000000),
  B8_1BIT(01100000),
  // Frame 2
  B8_1BIT(00000000),
  B8_1BIT(01000000),
  B8_1BIT(11000000),
  B8_1BIT(10000000),
  // Frame 3
  B8_1BIT(00000000),
  B8_1BIT(00000000),
  B8_1BIT(11000000),
  B8_1BIT(01100000),
  // Frame 4
  B8_1BIT(00000000),
  B8_1BIT(01000000),
  B8_1BIT(11000000),
  B8_1BIT(10000000)
};


#ifndef COMMON_H
#define COMMON_H
typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

union  var16
{
  u16 w;
  u8 b[2];
};
#endif
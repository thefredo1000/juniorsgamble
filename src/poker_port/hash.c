#include "tables.h"

int hash_quinary(const unsigned char q[], int k)
{
  int sum = 0;
  const int len = 13;
  int i;

  for (i = 0; i < len; i++)
  {
    sum += dp[q[i]][len - i - 1][k];

    k -= q[i];

    if (k <= 0)
    {
      break;
    }
  }

  return sum;
}
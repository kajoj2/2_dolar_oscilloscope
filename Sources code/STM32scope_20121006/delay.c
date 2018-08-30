/** \file
*/

void Delay1k(volatile int i)
{
    volatile int j;
    for (i;i>0;i--)
        for (j=1000;j>0;j--) {};
}

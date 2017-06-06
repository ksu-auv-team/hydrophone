#include <fftw3.h>
#include <sndfile.h>
#include <math.h>

int main (int argc, char *argv [])
{
    char        *infilename;
    SNDFILE     *infile = NULL;
    FILE        *outfile = NULL;
    SF_INFO     sfinfo;

    infile = sf_open("fft_test.flac", SFM_READ, &sfinfo);

    int N = pow(2, 12);

    for(int i = 0; i < sfinfo.frames; i += 88)
    {
        double samples[N];

        sf_seek(infile, i, SEEK_SET);
        sf_read_double(infile, samples, N);

        fftw_complex out[N];
        fftw_plan p;
        p = fftw_plan_dft_r2c_1d(N, samples, out, FFTW_ESTIMATE);

        fftw_execute(p);
        fftw_destroy_plan(p);

        /*
        for (int i=0; i<N/2; i++)
        {
            printf("%i\t%f\n", (int)(i*21.533203125), sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]));
        }
        */

        double total = 0;
        int prevmod = 1000;
        int band = 0;
        double highest[4][2] = {{0, 0},
                             {0, 0},
                             {0, 0},
                             {0, 0}};

        for(int i=0; i<N/2; i++)
        {
            total += sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);

            if (((int)(i*21.533203125) + 500) % 1000 < prevmod && i != 0)
            {
                //printf("%i\t%f\n", band * 1000, total);
                if(band != 0)
                {
                    if(total > highest[0][1])
                    {
                        highest[3][0] = highest[2][0];
                        highest[3][1] = highest[2][1];

                        highest[2][0] = highest[1][0];
                        highest[2][1] = highest[1][1];

                        highest[1][0] = highest[0][0];
                        highest[1][1] = highest[0][1];

                        highest[0][0] = band * 1000;
                        highest[0][1] = total;
                    }
                    else if(total > highest[1][1])
                    {
                        highest[3][0] = highest[2][0];
                        highest[3][1] = highest[2][1];

                        highest[2][0] = highest[1][0];
                        highest[2][1] = highest[1][1];

                        highest[1][0] = band * 1000;
                        highest[1][1] = total;
                    }
                    else if(total > highest[2][1])
                    {
                        highest[3][0] = highest[2][0];
                        highest[3][1] = highest[2][1];

                        highest[2][0] = band * 1000;
                        highest[2][1] = total;
                    }
                    else if(total > highest[3][1])
                    {
                        highest[3][0] = band * 1000;
                        highest[3][1] = total;
                    }
                }
                band++;
                total = 0;
            }
            prevmod = ((int)(i*21.533203125) + 500) % 1000;
        }


        printf("time: %i\n", i);
        printf("%f\t%f\n", highest[3][0], highest[3][1]);
        printf("%f\t%f\n", highest[2][0], highest[2][1]);
        printf("%f\t%f\n", highest[1][0], highest[1][1]);
        printf("%f\t%f\n", highest[0][0], highest[0][1]);

    }

    sf_close (infile);

    return 0;
}

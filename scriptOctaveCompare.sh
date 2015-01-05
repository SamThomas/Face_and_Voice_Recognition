#! /usr/bin/octave -qf
arg_list = argv();
pid = arg_list{1};
ordre = "kill -12 ";
fin ="";

pkg load signal
printf("0 %%\n"); 
main('sample1',1);
printf("25 %%\n");
main('sample1',2);
printf("50 %%\n");
main('sample1',3);
printf("75 %%\n");
main('sample1',4);
printf("100 %%\n");
printf("Envoi du signal\n");
#fin = strjoin({ordre,pid});
sprintf(fin,"%s %d",ordre,pid);
#printf("%s\n",fin);
system (fin);

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> /* fcntl */
#include <unistd.h>
#include <errno.h> /* errno */
#include <string.h>

#include "../Bateau/bateau.h"
#include "../Mer/mer.h"

/*
 *  Programme principal
 */

int main( int nb_arg , char * tab_arg[] ){
  char fich_mer[128] ;
  case_t marque = MER_CASE_LIBRE ;
  char nomprog[128] ;
  float energie = 0.0 ;
  int rowmer;
  int colmer;
  off_t taillemer;
  bateau_t * bateau;
  off_t offset;
  int taille_bateau;

  /* variables utilisation verrou */

  int fichier;
  char VerrouPosition[128];
  char VerrouBouclier[128];
  struct flock verrouBateauPose;
  struct flock verrouPlacementBateau;

  /* variables création bateau */

  /*----------*/

  strcpy( nomprog , tab_arg[0] ) ;

  if( nb_arg != 4 ){
      fprintf( stderr , "Usage : %s <fichier mer> <marque> <energie>\n",
         nomprog );
      exit(-1);
    }

  if( strlen(tab_arg[2]) !=1 ){
      fprintf( stderr , "%s : erreur marque <%s> incorrecte \n",
         nomprog , tab_arg[2] );
      exit(-1) ;
    }


  strcpy( fich_mer , tab_arg[1] );
  marque = tab_arg[2][0] ;
  sscanf( tab_arg[3] , "%f" , &energie );

  /* Initialisation de la generation des nombres pseudo-aleatoires */
  srandom((unsigned int)getpid());

  printf( "\n\n%s : ----- Debut du bateau %c (%d) -----\n\n",
    nomprog , marque , getpid() );

  printf("\n\n****** INITALISATION ******\n\n");

  /* Creation du verrou sur la mer pour le placement d'un bateau */

  fichier = open(fich_mer, O_RDONLY);

  sprintf(verrouPlacementBateau,"verrouPlacementBateau_%s",fich_mer);

  mer_dim_lire(fichier,&rowmer,&colmer);

  mer_lc2pos(fichier,rowmer,colmer,&taillemer);

  verrouPlacementBateau.l_type = F_WRLCK;
  verrouPlacementBateau.l_whence = 0;
  verrouPlacementBateau.l_start = 0;
  verrouPlacementBateau.l_len = taillemer;

  fcntl(fichier,F_SETLKW,verrouPlacementBateau);

  printf("Pose du verrou Position : OK\n");

  /* Pose du bateau sur la mer */

  bateau = bateau_new(coords_new(), marque, getpid());

  coords_printf(bateau->corps);

  verrouPlacementBateau.l_type = F_UNLCK;

  fcntl(fichier,F_SETLKW,verrouPlacementBateau);

  close(fichier);

  printf("\n\n****** DEBUT DE PARTIE ******\n\n");

  /* Creation d'un verrou sur un bateau */

  sprintf(verrouBateauPose,"verrouBateauPose_%s",fich_mer);

  fichier = open(fich_mer, O_RDONLY);

  coord_position_get(coords_coord_get(bateau->corps,1));
  offset = coord_position_get(coords_coord_get(bateau->corps,1));
  taille_bateau = coords_nb_get(bateau->corps);
  lseek(fichier,offset,SEEK_SET);

  verrouBateauPose.l_type = F_WRLCK;
  verrouBateauPose.l_whence = 1;
  verrouBateauPose.l_start = 0;
  verrouBateauPose.l_len = taille_bateau;

  fcntl(fichier,F_SETLKW,verrouBateauPose);
  close(fichier);

  printf("Pose du verrou Bouclier : OK");

  printf( "\n\n%s : ----- Fin du navire %c (%d) -----\n\n ",
    nomprog , marque , getpid() );

  exit(0);
}
#include "includes.h"




/**
 Fonction principale :
 *
 Permet de charger ou creer un réseau de neurones.
 Ce réseau composé de neurones avec des liens pondérés 
 permet de reconnaitre des chiffres.
**/
int main (void)
{
	srand((unsigned int) time(NULL));
	int i, j, k, indi, cond;
	float somme;
    
    //On charge ou crée le réseau
	printf("Voulez-vous charger un reseau? ");
	scanf("%d", &cond);
    
	if (cond)
		chargerReseau();
    
	else
		creerReseau();
    
    //Faut-il faire un apprentissage
	printf("\nVoulez-vous faire une phase d'apprentissage? ");
	scanf("%d", &cond);
    
	if (cond) {
        printf("Combien de phases d'apprentissage? ");
        scanf("%d", &nombreApprentissages);
        
		//On charge en mémoire les images.
		printf("Mise en memoire :\n");
        
		for (j = 0; j < NOMBRE_SERIES; j++) {
			printf("Serie %03d | ", j);
            
			for (i = 0; i < NOMBRE_CARACS; i++) {
				printf("-");
				imageToVect(i, j, 0);
			}
            
			printf("\n");
		}
        
        //On lance l'apprentissage
		printf("\n\nApprentissage :\n");
        
		for (i = 0; i < nombreApprentissages; i++) {
			cond = rand() % NOMBRE_SERIES;
            indi = rand() % NOMBRE_CARACS;
			printf("%05d | Serie %03d_%d | ", i, cond, indi);
			somme = calculerErreur(indi, cond);
			
			for (j = 0; j < round(somme * 10); j++)
				printf("-");
            
            printf("\n");
		}
        
        //On affiche le nombre d'erreurs pour chaque série de chiffres
		printf("\n\nIllustration reconnaissance :\n");
		cond = 0;
        
		for (i = 0; i < NOMBRE_SERIES; i++) {
			printf("Serie %03d | ", i);
			somme = 0;
            
			for (j = 0; j < NOMBRE_CARACS; j++) {
				calculerEtapes(entrees[j][i]);
				indi = 0;
                
				for (k = 0; k < _sorties; k++) {
					if (gives[_couches][k] > gives[_couches][indi])
						indi = k;
				}
                
				if (indi == j)
					somme++;
                
				printf("%d", indi);
			}
            
			cond += (int) somme;
			printf(" | ");
            
			for (j = 0; j < NOMBRE_CARACS - (int) somme; j++)
				printf("-");
            
			printf("\n");
		}
        
        //On affiche le pourcentage de réussite à l'apprentissage
		printf("Taux de reussite apprentissage : %f\n",
               (float) cond * 100 / (NOMBRE_SERIES * NOMBRE_CARACS));
        
        //Faut-il sauvegarder le réseau à la fin de cet apprentissage
		printf("\nVoulez-vous sauvegarder le reseau? ");
		scanf("%d", &cond);
        
		if (cond)
			sauverReseau();
	}
    
    //Faut-il lancer la reconnaissance sur les séries suivantes
    printf("\nVoulez-vous reconnaitre les autres series? ");
    scanf("%d", &cond);
    somme = 0;
        
    if (cond) {
        for (k = NOMBRE_SERIES; k < NBRTOT_SERIES; k++) {
            printf("Serie %03d | ", k);
            cond = 0;
            
            //On charge en mémoire les images et on calcule la sortie du réseau
            //avec la matrice des pixels de l'image comme entrée
            //On cherche la sortie ayant la plus grande valeur et on compare avec celle désirée
            for (i = 0; i < 10; i++) {
                imageToVect(i, k, 1);
                calculerEtapes(entrees[i][0]);
                indi = 0;
                
                for (j = 0; j < _sorties; j++) {
                    if (gives[_couches][j] > gives[_couches][indi])
                        indi = j;
                }
                
                printf("%d", indi);
                
                if (indi != i)
                    cond++;
            }
            
            printf(" | %d erreurs\n", cond);
            somme += cond;
		}
        
        //On affiche le pourcentage de réussite à la reconnaissance
		printf("Taux de reussite de la reconnaissance : %f\n",
            (float) 100 - somme * 100 / ((NBRTOT_SERIES - NOMBRE_SERIES) * NOMBRE_CARACS));
    }

	detruireReseau();
    return 0;
}






/**
 Fonction de création du réseau
 *
 Combien de couches cachées et de neurones dans ces couches
 On alloue les cases mémoires nécéssaires au bon fonctionnement.
 On affecte des poids aléatoires sur chaque lien
**/
void creerReseau (void) {
	int i, j, k;
    
    printf("Nombre de couches cachees : ");
    scanf("%d", &_couches);
    
	couches = (int *) malloc((_couches + 2) * sizeof(int));
    couches[0] = _entrees;
    couches[_couches + 1] = _sorties;
    
	for (i = 1; i <= _couches; i++) {
        printf("Nombre neurones couche %d : ", i);
		scanf("%d", &couches[i]);
	}
    
    poids = (float ***) malloc((_couches + 1) * sizeof(float **));
    ajout = (float ***) malloc((_couches + 1) * sizeof(float **));
    gives = (float **) malloc((_couches + 1) * sizeof(float *));
    delta = (float **) malloc((_couches + 1) * sizeof(float *));
    
    for (i = 0; i <= _couches; i++) {
        poids[i] = (float **) malloc(couches[i + 1] * sizeof(float *));
        ajout[i] = (float **) malloc(couches[i + 1] * sizeof(float *));
        gives[i] = (float *) malloc(couches[i + 1] * sizeof(float));
        delta[i] = (float *) malloc(couches[i + 1] * sizeof(float));
        
        for (j = 0; j < couches[i + 1]; j++) {
            poids[i][j] = (float *) malloc((couches[i] + 1) * sizeof(float));
            ajout[i][j] = (float *) malloc((couches[i] + 1) * sizeof(float));
            
            //On remplie les poids sans oublier les biais
            for (k = 0; k <= couches[i]; k++) {
                poids[i][j][k] = poidsAleatoire();
                ajout[i][j][k] = 0;
            }
        }
    }
}






/**
 Fonction de calcul de l'erreur concernant une entrée donnée correspondant à un chiffre
 *
 Fait la somme pour chaque sortie des carrés de la différence entre la sortie théorique
 et la sortie obtenue en prenant en compte l'image correspondante
 **/
float calculerErreur (int chiffre, int numero) {
	int k;
    float somme = 0;
    
    modifierPoids(chiffre, numero);
    
	for (k = 0; k < _sorties; k++)
		somme += fabsf(gives[_couches][k] - (chiffre == k ? 1 : 0));
    
	return somme;
}






/**
 Fonction de destruction du réseau
 *
 Désalloue corresctement les cases mémoires allouées avec malloc,
 afin d'éviter les fuites mémoires.
 **/
void detruireReseau (void) {
	int i, j;
    
	for (i = 0; i <= _couches; i++) {
        for (j = 0; j < couches[i + 1]; j++) {
            free(poids[i][j]);
            free(ajout[i][j]);
        }
        
        free(poids[i]);
        free(ajout[i]);
        free(gives[i]);
        free(delta[i]);
    }
    
    free(poids);
    free(ajout);
    free(gives);
    free(delta);
    free(couches);
}






/**
 Fonction de calcul intermédiaire du réseau
 *
 Calcule les sorties de chaque neurones, en utilisant le produit scalaire sur le vecteur 
 d'agrégation (les sorties des neurones de la couche précédenté, ou de l'entrée 
 dans le cas de la couche d'entrée) et du vecteur des poids du neurones.
 Applique ensuite la fonction d'activation (ici sigmoide)
 **/
void calculerEtapes (float entree[]) {
    int i, j, k;
    float somme;
    
    for (i = 0; i <= _couches; i++) {
        for (j = 0; j < couches[i + 1]; j++) {
            somme = 0;
            
            for (k = 0; k < couches[i]; k++)
                somme += poids[i][j][k] * (i == 0 ? entree[k] : gives[i - 1][k]);
            
            //On prend en compte le biais
            somme += poids[i][j][couches[i]];
            gives[i][j] = f(somme);
        }
    }
}






/**
 Fonction de modification des poids du réseau
 *
 Cette fonction applique l'algorithme de rétropropagation du gradient sur un 
 réseaux de neurones multicouches, avec moment d'inertie pour avoir de meilleurs résultats 
 et ne pas rester bloqué dans un minimum local.
 En premier lieu on calcule les sorties intermédiaires.
 **/
void modifierPoids (int chiffre, int numero) {
    int i, j, k;
    float somme;
    
    //Calcul des sorties intermédiaires pour chaque neurones
    calculerEtapes(entrees[chiffre][numero]);
    
    //On calcule le delta pour chaque neurone de la couche de sortie
    for (i = 0; i < _sorties; i++)
        delta[_couches][i] = df(gives[_couches][i]) * ((chiffre == i ? 1 : 0) - gives[_couches][i]);
    
    //On calcule le delta par propagation sur les couches précédentes
    for (i = _couches - 1; i >= 0; i--) {
		for (j = 0; j < couches[i + 1]; j++) {
			somme = 0;
			for (k = 0; k < couches[i + 2]; k++)
				somme += delta[i + 1][k] * poids[i + 1][k][j];
            
			delta[i][j] = df(gives[i][j]) * somme;
		}
	}
    
    //On modifie les poids avec pas et moment
	for (i = 0; i <= _couches; i++) {
		for (j = 0; j < couches[i + 1]; j++) {
			for (k = 0; k < couches[i]; k++) {
				somme = pas * (i == 0 ? entrees[chiffre][numero][k] : gives[i - 1][k]) * delta[i][j];
				poids[i][j][k] += somme + momentum * ajout[i][j][k];
				ajout[i][j][k] = somme;
			}
            
            //On fait de même pour le biais
			somme = pas * delta[i][j];
			poids[i][j][couches[i]] += somme + momentum * ajout[i][j][k];
			ajout[i][j][couches[i]] = somme;
		}
	}
}






/**
 Fonction de sauvegarde du réseau
 *
 Recopie le nombre de couches cachées, le nombre de neurones dans chaque couche
 ainsi que tous les poids du réseau dans un fichier afin de le réutiliser par la suite.
 **/
void sauverReseau (void) {
	char nom[255];
    char fic[255];
	char str[255];
	int i, j, k;
	FILE *fichier;
    
    //Nom du fichier
    printf("Nom du fichier : ");
	scanf("%s", nom);
    sprintf(fic, "data/%s", nom);
	fichier = fopen(fic, "w");
    
    //Vérification d'ouverture
	if (fichier == NULL) {
		printf("\nERREUR : Creation fichier impossible\n\n");
		_pause();
		exit(0);
	}
    
	sprintf(str, "%d\n", _couches);
	fputs(str, fichier);
    
	for (i = 1; i <= _couches; i++) {
		sprintf(str, "%d\n", couches[i]);
		fputs(str, fichier);
	}
    
	for (i = 0; i <= _couches; i++) {
        for (j = 0; j < couches[i + 1]; j++) {
            for (k = 0; k <= couches[i]; k++) {
				sprintf(str, "%f\n", poids[i][j][k]);
				fputs(str, fichier);
            }
        }
	}

	fclose(fichier);
}






/**
 Fonction de chargement du réseau
 *
 On charge le fichier, on récuppère le nombre de couches et de neurones.
 On alloue le bon nombre de cases mémoires, et on charges les poids.
 TODO : Il faudrait peut être vérifier la validité du fichier, et aussi enregistrer 
 les ajouts même si leur influence est négligeable.
 **/
void chargerReseau (void) {
	char nom[255];
    char fic[255];
	char str[255];
	int i, j, k;
	FILE *fichier;
    
    //Nom du fichier
    printf("Nom du fichier : ");
	scanf("%s", nom);
    sprintf(fic, "data/%s", nom);
	fichier = fopen(fic, "r");
    
    //Vérification d'ouverture
	if (fichier == NULL) {
		printf("\nERREUR : Lecture fichier impossible_\n\n");
		_pause();
		exit(0);
	}
    
	fgets(str, sizeof(str), fichier);
	sscanf(str, "%d", &_couches);

	couches = (int *) malloc((_couches + 2) * sizeof(int));
    couches[0] = _entrees;
    couches[_couches + 1] = _sorties;
    poids = (float ***) malloc((_couches + 1) * sizeof(float **));
    ajout = (float ***) malloc((_couches + 1) * sizeof(float **));
    gives = (float **) malloc((_couches + 1) * sizeof(float *));
    delta = (float **) malloc((_couches + 1) * sizeof(float *));
    
	for (i = 1; i <= _couches; i++) {
		fgets(str, sizeof(str), fichier);
		sscanf(str, "%d", &couches[i]);
	}
    
	for (i = 0; i <= _couches; i++) {
		poids[i] = (float **) malloc(couches[i + 1] * sizeof(float *));
        ajout[i] = (float **) malloc(couches[i + 1] * sizeof(float *));
        gives[i] = (float *) malloc(couches[i + 1] * sizeof(float));
        delta[i] = (float *) malloc(couches[i + 1] * sizeof(float));
        
        for (j = 0; j < couches[i + 1]; j++) {
            poids[i][j] = (float *) malloc((couches[i] + 1) * sizeof(float));
            ajout[i][j] = (float *) malloc((couches[i] + 1) * sizeof(float));
            
            for (k = 0; k <= couches[i]; k++) {
				fgets(str, sizeof(str), fichier);
				sscanf(str, "%f", &poids[i][j][k]);
				ajout[i][j][k] = 0;
            }
		}
	}
    
	fclose(fichier);
}






/**
 Fonction de conversion image BMP vers vecteur entré.
 *
 ATTENTION : Fonction propre aux images BMP 16x16 codées en 32 bits UNIQUEMENT
 Récupère chaque pixel, lui attribue le niveau de gris correspondant
 Place la valeur du niveau de gris entre 0 (blanc) et 1 (noir) dans le tableaux des entrees
 **/
void imageToVect (int chiffre, int numero, int reconnaissance) {
    FILE *fichier;
    int chr;
    int i;
    float somme = 0;
    float bgr[4] = {0, 0.299, 0.587, 0.114};
    char nom[255];
    
    //Récupere l'image
    sprintf(nom, "imgs/%d_%d.bmp", chiffre, numero);
    fichier = fopen(nom, "rb");
    
    //Vérification d'ouverture
	if (fichier == NULL) {
		printf("\nERREUR : Lecture fichier impossible\n\n");
		_pause();
		exit(0);
	}
    
    //On récupére la taille de l'entete et on saute jusqu'aux données brutes
    fseek(fichier, 10, SEEK_SET);
    chr = fgetc(fichier);
    fseek(fichier, chr, SEEK_SET);
	i = 1;
    
    //Lecture de l'image pixel par pixel
    //Chaque pixel est représenté par 4 octets (RGBA)
    //On n'utilise pas la composante alpha des pixels
    while ((chr = fgetc(fichier)) != EOF) {
		if (i % 4 != 0)
			somme += bgr[i % 4] * chr;
        
		if (i % 4 == 0) {
			entrees[chiffre][reconnaissance == 0 ? numero : 0][256 - i/4] = 1 - somme / 255;
			somme = 0;
		}
        
		i++;
    }
    
    fclose(fichier);
}






/**
 Fonction qui renvoie un flottant aléatoire entre -1 et 1
 **/
float poidsAleatoire (void) {
    return ((float) 2. * (rand() - (RAND_MAX / 2.)) / RAND_MAX);
}






/**
 Fonction d'activation : sigmoide
 *
 1
 f : x -> -------------
 1 + exp(-x)
 **/
float f (float x) { return 1 / (1 + exp(-x)); }






/**
 Fonction dérivée de la fonction d'activation
 *
 Cas particulier pour la fonction sigmoide, on a
 f'(x) = f(x) * (1 - f(x))
 **/
float df (float y) { return y * (1 - y); }






/**
 Fonction pause
 *
 TODO : Pas encore trouvé comment faire sous MAC
 **/
void _pause(void) {
    //system("PAUSE");
}
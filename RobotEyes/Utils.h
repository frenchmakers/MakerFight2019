/***************************************************
 * Utilitaires
 * 
 * Ecrit par Yan Grenier pour Frenck Makers.
 * MIT license, all text above must be included in any redistribution
 */

#ifndef Utils_h
#define Utils_h

/**
 * Classe prenant en charge le temps qui passe
 */
class Timeline {
  private:
    unsigned long m_time;
    unsigned long m_elapsed;
  public:
    Timeline();

    /**
     * Réinitialisation des compteurs
     */
    void reset();

    /**
     * Retour le temps passé actuel
     */
    unsigned long getElapsed();

    /**
     * Indique si temps espéré est passé
     * 
     * Le temps passé est recalculé
     */
    bool isTimePasted(unsigned long expected);
    
    /**
     * Compte le nombre de fois que le temps espéré est passé
     * 
     * Le temps passé est recalculé
     */
    unsigned long timePasted(unsigned long expected);
};

#endif // Utils_h

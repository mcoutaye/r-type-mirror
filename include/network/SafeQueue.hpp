/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** SafeQueue
*/

#pragma once

#include <queue>
#include <mutex>

namespace Nwk {

/**
 * @brief File d'attente thread-safe générique
 *
 * @tparam T Type des éléments stockés dans la file
 *
 * Utilisée pour communiquer entre les threads réseau et la logique du jeu
 * (ex: inputs reçus, paquets à envoyer, snapshots d'entités).
 * Protégée par un mutex pour un accès concurrentiel sécurisé.
 */
template <typename T>
class SafeQueue {
public:
    /// Constructeur par défaut
    SafeQueue() = default;

    /**
     * @brief Ajoute un élément dans la file
     * @param value Valeur à pousser (copie)
     */
    void push(const T& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
    }

    /**
     * @brief Retire un élément de la file (si disponible)
     * @param value Référence où stocker l'élément retiré
     * @return true si un élément a été retiré, false si la file était vide
     */
    bool pop(T& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty())
            return false;
        value = queue_.front();
        queue_.pop();
        return true;
    }

private:
    std::queue<T> queue_;   ///< File interne non protégée
    std::mutex mutex_;      ///< Mutex pour la synchronisation
};

} // namespace Nwk

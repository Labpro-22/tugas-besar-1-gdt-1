#ifndef CARD_DECK_HPP
#define CARD_DECK_HPP

#include <vector>
#include <algorithm>
#include <random>
#include <stdexcept>

template <typename T>
class CardDeck {
private:
    std::vector<T*> drawPile;
    std::vector<T*> discardPile;

public:
    CardDeck() {}
    ~CardDeck() {
        for (T* card : drawPile) delete card;
        for (T* card : discardPile) delete card;
    }

    void addCard(T* card) {
        drawPile.push_back(card);
    }

    T* draw() {
        reshuffleIfEmpty();
        if (drawPile.empty()) {
            throw std::runtime_error("Deck kosong!");
        }
        T* card = drawPile.back();
        drawPile.pop_back();
        return card;
    }

    void discard(T* card) {
        discardPile.push_back(card);
    }

    void shuffle() {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::shuffle(drawPile.begin(), drawPile.end(), rng);
    }

    void reshuffleIfEmpty() {
        if (drawPile.empty() && !discardPile.empty()) {
            drawPile = discardPile;
            discardPile.clear();
            shuffle();
        }
    }

    bool isEmpty() const {
        return drawPile.empty() && discardPile.empty();
    }

    const std::vector<T*>& getDrawPile() const {
        return drawPile;
    }

    const std::vector<T*>& getDiscardPile() const {
        return discardPile;
    }

    int size() const {
        return (int)(drawPile.size() + discardPile.size());
    }

    T* takeByName(const std::string& name) {
        for (auto it = drawPile.begin(); it != drawPile.end(); ++it) {
            if ((*it)->getCardName() == name) {
                T* card = *it;
                drawPile.erase(it);
                return card;
            }
        }
        for (auto it = discardPile.begin(); it != discardPile.end(); ++it) {
            if ((*it)->getCardName() == name) {
                T* card = *it;
                discardPile.erase(it);
                return card;
            }
        }
        return nullptr;
    }

    void loadState(std::vector<T*> newDrawPile, std::vector<T*> newDiscardPile) {
        drawPile = std::move(newDrawPile);
        discardPile = std::move(newDiscardPile);
    }
};

#endif

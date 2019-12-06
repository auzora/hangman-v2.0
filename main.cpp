#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <ctime>
#include <sstream>
#include <cctype>
#include <iterator>
#include <cstring>
#include <random>

using   Word    = std::pair<std::string, std::string>;
class Picture{
    std::string image;
    public:
        friend std::istream& operator>>(std::istream& s, Picture& data)
        {
            std::string line;
            while(std::getline(s, line))
            {
                data.image  += line;
                data.image  += "\n";
                if (line == "")
                {
                    break;
                }
            }
            return s;
        }
        friend std::ostream& operator<<(std::ostream& s, Picture const& data)
        {
            s << data.image;
            return s;
        }
};
class Gallows{
    std::vector<Picture>                images;
    public:
        Gallows()
        {
            std::ifstream   picStream("hm.data");
            images.insert(std::begin(images),
                          std::istream_iterator<Picture>(picStream),
                          std::istream_iterator<Picture>());
        }

        void drawGallows(int rWrongGuesses)
        {
            if (rWrongGuesses < images.size())
            {
                std::cout << images[rWrongGuesses];
            }
        }
};
class WordMap{
    using   Words   = std::vector<Word>;

    Words               words;
    std::random_device  randDevice;
    std::mt19937        generator;

    public:
        WordMap()
            : generator(randDevice())
        {
            std::ifstream   wordStream("hm.words");
            std::string     line;

            while(std::getline(wordStream, line))
            {
                std::size_t     comma   = line.find(',');
                std::string     word    = line.substr(0, comma);
                std::string     def     = line.substr(comma);

                words.emplace_back(word, def);
            }
        }
        Word const& getRandomWord()
        {
            std::uniform_int_distribution<int>  distr(0, words.size());
            return words[distr(generator)];
        }
};
class Game{
    const int kMaxWrongGuesses = 8; //maximum number of incorrect guesses allowed

    Word const&     word;
    int             wrongGuesses;
    std::string     soFar;
    std::string     GuessedLetters;

    public:
        Game(Word const& word)
            : word(word)
            , wrongGuesses(0)
            , soFar(word.first.size(), '-')
            , GuessedLetters("")
        {
            std::cout << "Welcome to Hangman. Good luck!\n";
            std::cout << "Here is your hint: " << word.second << " \n";
        }
        ~Game()
        {
            if (wrongGuesses == kMaxWrongGuesses)
            {
                std::cout << "\nYou've been hanged!";
            }
            else
            {
                std::cout << "\nYou guessed it!";
            }

            std::cout << "\nThe word was " << word.first << "\n";
        }

        bool makeGuess()
        {
            prompt();
            char guess = getGuess();
            checkGuess(guess);

            return ((wrongGuesses < kMaxWrongGuesses) && (soFar != word.first));
        }

        int getWrongGuesses() const
        {
            return wrongGuesses;
        }

    private:
        void prompt()
        {
            std::cout   << "\n\nYou have " << (kMaxWrongGuesses - wrongGuesses)
                        << " incorrect guesses left.\n"
                        << "\nYou've used the following letters:\n" << GuessedLetters << "\n"
                        << "\nSo far, the word is:\n" << soFar << "\n";
        }

        char getGuess()
        {
            char guess;
            std::cout << "\n\nEnter your guess: ";
            std::cin >> guess;
            guess = toupper(guess); // make uppercase since secret word is in uppercase
            while (GuessedLetters.find(guess) != std::string::npos)
            {
                std::cout << "\nYou've already guessed " << guess << "\n"
                          << "Enter your guess: ";
                std::cin >> guess;
                guess = toupper(guess);
            }

            GuessedLetters += guess;
            return guess;
        }

        void checkGuess(char guess)
        {
            if (word.first.find(guess) != std::string::npos)
            {
                std::cout << "That's right! " << guess << " is in the word.\n";

                //update soFar to include newly guessed letter
                for (int i = 0; i < word.first.length(); ++i)
                {
                    if (word.first[i] == guess)
                    {
                        soFar[i] = guess;
                    }
                }
            }
            else
            {
                std::cout << "Sorry, " << guess << " isn't in the word.\n";
                ++wrongGuesses;
            }
        }
};
int main(){
    Gallows     gallows;
    WordMap     wordMap;

    //setup
    Word const& wordToGuess = wordMap.getRandomWord();
    Game        game(wordToGuess);

    while(game.makeGuess())
    {
        gallows.drawGallows(game.getWrongGuesses());
    }
}
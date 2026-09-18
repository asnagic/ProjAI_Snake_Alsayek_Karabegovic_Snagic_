#pragma once
#include <gui/Canvas.h>
#include <gui/Shape.h>
#include <gui/Image.h>
#include <gui/DrawableString.h>
#include <gui/Sound.h>
#include <math/math.h>
#include <random>
#include <random>
#include <vector>
#include <functional>
#include <algorithm>
#include <utility>
#include "Difficulty.h"
#include "AIsolver.h"
#include <cmath>
#include <algorithm> 
#include <vector>    
#include <gui/Color.h>

class ViewSnake : public gui::Canvas
{
protected:
    enum class Direction { NONE = 0, LEFT, RIGHT, UP, DOWN };
    enum class ItemType { Apple = 0, GoldenApple, Timer, Bomb, Speed };
    struct Particle {
        gui::Point pos = { 0, 0 };
        gui::Point velocity = { 0, 0 };
        float life = 0.0f;
    };
    std::vector<gui::Point> _history;
    gui::Point _snakePos;
    gui::CoordType _stepSize = 5.0;
    Direction _direction = Direction::NONE;

    bool _gameInProgress = false;
    bool _gameOver = false;
    bool _aiMode = false;
    bool _bombsEnabled = true;
    std::function<void()>* _pStartGame = nullptr;   // postavlja MainView
    int _respawnTimer = 0;
    const int _respawnDelay = 50; 
    const float _safeSpawnRadius = 100.0f; 
    bool _itemActive = false;
    bool _modeSelection = true;   // ekran izbora moda se prikazuje odmah pri pokretanju


    size_t _gridSize = 25;
    size_t _currentSnakeSize = 15;
    size_t _growthFactor = 1;

    int _secondsRemaining = 60;
    int _tickCounter = 0;

    gui::CoordType _headRadius = 18.;
    gui::CoordType _bodyRadius = 13.;

    gui::Image _apple{ ":jabukaaa" };
    gui::Image _goldenApple{ ":zlatnajabukaa" };
    gui::Image _timerImg{ ":timer" };
    gui::Image _bombImg{ ":dynamite" };
    gui::Image _imgGameOver{ ":gameOver" };
    gui::Image _imgPlayer{ ":human" };
	gui::Image _imgAI{ ":ai" };
    gui::Image _backgroundImg{ ":backgroundSnake" };
    gui::Image _speedImg{ ":speed" };
    gui::Image _heartFull{":heartFull"};
    gui::Image _heartEmpty{":heartEmpty"};
    

    gui::Rect _itemRect;
    ItemType _currentItem = ItemType::Apple; 
    ItemType _lastItemType = ItemType::Apple;
    gui::CoordType _itemSize = 55.0;



    std::random_device _rd;
    std::mt19937 _rng;
    std::uniform_int_distribution<int> _randomItem{ 0, 4 };
    std::uniform_int_distribution<int> _randomX;
    std::uniform_int_distribution<int> _randomY;

    gui::Size _size = { 0, 0 };
    td::INT4 _score = 0;
    td::INT4 _highScore = 0;
    int _lives = 3;

    gui::Sound _appleBite{ ":appleBite" };
    gui::Sound _bomb{ ":bomb" };
    gui::Sound _background{ ":backgroundSound" };
    gui::Sound _sndGameOver{ ":gameOver" };
    gui::Sound _sndGoldenApple{ ":goldenApple" };
    gui::Sound _sndTimer{ ":timer" };
    gui::Sound _sndSpeed{ ":speedboost" };

    std::function<void()>* _pUpdateUI;
    std::function<void(td::INT1, int, int)>* _pUpdateStatus;
    Difficulty _difficulty = Difficulty::Medium;
    float _speedMultiplier = 1.0f;
    int _goldenAppleTimer = 0;
    int _bombTimer = 0;
    int _timerItemTimer = 0;

    float _rainbowTimer = 0.0f;
    float _speedBoostTimer = 0.0f;
    float _deathFlashTimer = 0.0f;
    float _screenShakeTimer = 0.0f;
    float _invulnerabilityTimer = 0.0f;

    bool _rainbowMode = false;
    bool _speedBoostActive = false;


    std::vector<Particle> _particles;

    float _applePulseTime = 0.0f;

public:
    ViewSnake(std::function<void()>* pUI, std::function<void(td::INT1, int, int)>* pStatus)
        : gui::Canvas({ gui::InputDevice::Event::Keyboard })
        , _rng(_rd())
        , _pUpdateUI(pUI)
        , _pUpdateStatus(pStatus)
    {
        enableResizeEvent(true);
        auto props = getAppProperties();
        _highScore = props->getValue("highScore", (td::INT4)0);
        _randomItem = std::uniform_int_distribution<int>(0, 4);
    }

    bool isGameOver() const { return _gameOver; }

    float _itemLifetime = 0.0f;

    void spawnSpecificItem(ItemType type, int lifetimeSeconds = 0)
    {
        _currentItem = type;
        _itemLifetime = (float)lifetimeSeconds;
        
        if (_size.width <= _itemSize || _size.height <= _itemSize)
        {
            _itemRect = { (_size.width - _itemSize) / 2.0f, (_size.height - _itemSize) / 2.0f, _itemSize, _itemSize };
            this->reDraw();
            return;
        }

        bool validPos = false;
        while (!validPos)
        {
            gui::CoordType x = (gui::CoordType)_randomX(_rng);
            gui::CoordType y = (gui::CoordType)_randomY(_rng);
            gui::Rect tempRect{ x, y, x + _itemSize, y + _itemSize };

            validPos = true;
            for (auto& s : _history)
            {
                gui::Rect snakePart(s.x - _headRadius, s.y - _headRadius, s.x + _headRadius, s.y + _headRadius);
                if (tempRect.intersects(snakePart))
                {
                    validPos = false;
                    break;
                }
            }
            if (validPos)
            {
                _itemRect = tempRect;
                break;
            }
        }
        _itemActive = true;
        this->reDraw();
    }


    void setAI(bool enabled)
    {
        _aiMode = enabled;
    }

    

    void enableBombs(bool enabled) { _bombsEnabled = enabled; }
    bool isGameInProgress() const { return _gameInProgress; }
    bool isAI() const { return _aiMode; }

    void setStartGameCallback(std::function<void()>* pStart) { _pStartGame = pStart; }

    void setDifficulty(Difficulty d) {
        _difficulty = d;

        switch (d) {
        case Difficulty::Easy:
            _screenShakeTimer = 0.1f;
            _gridSize = 25;
            _stepSize = 3.0f;
            _secondsRemaining = 180;
            _bombsEnabled = false;
            _growthFactor = 1;
            break;
        case Difficulty::Medium:
            _screenShakeTimer = 0.1f;
            _gridSize = 20;
            _stepSize = 4.0f;
            _secondsRemaining = 180;
            _bombsEnabled = true;
            _growthFactor = 1;
            break;
        case Difficulty::Hard:
            _screenShakeTimer = 0.1f;
            _gridSize = 20;
            _stepSize = 4.2f;
            _secondsRemaining = 180;
            _bombsEnabled = true;
            _growthFactor = 1;
            break;
        }
    }
    int getRandomSeconds(int minSec, int maxSec) {
        std::uniform_int_distribution<int> dist(minSec, maxSec);
        return dist(_rng);
    }

    void setSpeedMultiplier(float m) { _speedMultiplier = m; }

    void addExtraTime(int seconds) {
        _secondsRemaining += seconds;
        if (_pUpdateStatus) (*_pUpdateStatus)(2, _score, _secondsRemaining);
    }

    void newGame()
    {
        _snakePos = { _size.width / 2, _size.height / 2 };
        _direction = Direction::RIGHT;
        _gameOver = false;
        _gameInProgress = true;
        _score = 0;
        _lives = 3;
        _secondsRemaining = 60;
        _tickCounter = 0;
        _currentSnakeSize = 10;

        _history.clear();
        _history.push_back(_snakePos);

        if (_size.width > 0 && _size.height > 0)
        {
            _randomX = std::uniform_int_distribution<int>((int)_itemSize, (int)_size.width - (int)_itemSize);
            _randomY = std::uniform_int_distribution<int>((int)_itemSize, (int)_size.height - (int)_itemSize);
        }

        spawnItem();
        _background.play(true);

        if (_pUpdateStatus) (*_pUpdateStatus)(-1, 0, _secondsRemaining);
        this->reDraw();
        _goldenAppleTimer = getRandomSeconds(10, 15); //golden apple timer
        _bombTimer = (_difficulty == Difficulty::Medium) ? 20 : (_difficulty == Difficulty::Hard ? 10 : 0);
        _timerItemTimer = 15;
        _direction = Direction::RIGHT;
        _itemActive = true;

    }
   
    void timeStep()
    {
        const float dT = 0.02f;
        
        if (_gameOver)
            return;

        _applePulseTime += 0.1f;

        for (auto& p : _particles)
        {
            p.pos.x += p.velocity.x;
            p.pos.y += p.velocity.y;
            p.life -= dT;
        }

        _particles.erase(
            std::remove_if(_particles.begin(), _particles.end(),
                [](Particle& p) { return p.life <= 0; }),
            _particles.end());
        
        if (_speedBoostActive)
        {
            _speedBoostTimer -= dT;
            if (_speedBoostTimer <= 0)
            {
                _speedBoostActive = false;
                _speedMultiplier = 1.0f;
            }
        }

        if (_rainbowMode)
        {
            _rainbowTimer += dT;
            if (_rainbowTimer > 5.0f)
                _rainbowMode = false;
        }
        _tickCounter++;
        if (_tickCounter >= 50) // 1 second passed
        {
            _secondsRemaining--;
            _tickCounter = 0;
            if (_pUpdateStatus) (*_pUpdateStatus)((td::INT1)-1, _score, _secondsRemaining);

            if (_secondsRemaining <= 0) {
                setGameOver();
                return;
            }

            // Update timers for special items
            _goldenAppleTimer--;
            _bombTimer--;
            _timerItemTimer--;

            if (_goldenAppleTimer <= 0)
            {
                spawnSpecificItem(ItemType::GoldenApple);
                _goldenAppleTimer = getRandomSeconds(10, 15); 
            }
            if (_screenShakeTimer > 0)
                _screenShakeTimer -= 0.02f;
            if (_timerItemTimer <= 0)
            {
                spawnSpecificItem(ItemType::Timer);
                _timerItemTimer = 15; 
            }
            if (_difficulty == Difficulty::Medium && _bombTimer <= 0 && _currentItem != ItemType::Bomb)
            {
                spawnSpecificItem(ItemType::Bomb, 5); //disappears after 5 sec
                _bombTimer = 20;
            }
            else if (_difficulty == Difficulty::Hard && _bombTimer <= 0 && _currentItem != ItemType::Bomb)
            {
                spawnSpecificItem(ItemType::Bomb, 5);
                _bombTimer = 10;
            }
        }
        
        // Inside timeStep()

        static int frameCounter = 0;
        frameCounter++;

        if (_aiMode && _gameInProgress) {
            int currentGridX = (int)(_snakePos.x / _gridSize);
            int currentGridY = (int)(_snakePos.y / _gridSize);

            static int lastGridX = -1;
            static int lastGridY = -1;

            if (currentGridX != lastGridX || currentGridY != lastGridY) {
                _direction = computeAIMove();
                lastGridX = currentGridX;
                lastGridY = currentGridY;
            }
        }

        if (_itemLifetime > 0.0f)
        {
            _itemLifetime -= dT;
            if (_itemLifetime <= 0.0f && _currentItem == ItemType::Bomb)
            {
                spawnItem(); //replace bomb with normal item
            }
        }

        moveSnake();
        checkCollision();
        this->reDraw();
    }


    void setGameOver()
    {
        _gameOver = true;
        _gameInProgress = false;
        _background.stop();
        _screenShakeTimer = 0.2f;
        _deathFlashTimer = 0.2f;

        if (_score > _highScore) {
            _highScore = _score;
            getAppProperties()->setValue("highScore", _highScore);
        }
        if (_pUpdateStatus) (*_pUpdateStatus)(3, _score, _secondsRemaining);
        if (_pUpdateUI) (*_pUpdateUI)();
        this->reDraw();
    }

protected:
    void spawnItem()
    {
        if (_size.width <= _itemSize || _size.height <= _itemSize)
            return;

        int type = _randomItem(_rng);
        if (_difficulty == Difficulty::Easy && type == (int)ItemType::Bomb)
            type = (int)ItemType::Apple;

        if (_speedBoostActive && type == (int)ItemType::Speed)
            type = (int)ItemType::Apple;

        if (type == (int)ItemType::Bomb && _lastItemType == ItemType::Bomb)
            type = (int)ItemType::Apple;

        _currentItem = (ItemType)type;
        _lastItemType = _currentItem;

        if (_currentItem == ItemType::Bomb) {
            _itemLifetime = 3.0f;
        }

        bool validPos = false;

        while (!validPos)
        {
            gui::CoordType x = (gui::CoordType)_randomX(_rng);
            gui::CoordType y = (gui::CoordType)_randomY(_rng);

            gui::Rect tempRect{ x, y, x + _itemSize, y + _itemSize };

            validPos = true;

            float dx = (float)(_snakePos.x - (x + _itemSize / 2));
            float dy = (float)(_snakePos.y - (y + _itemSize / 2));
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < _safeSpawnRadius)
                validPos = false;

            for (auto& s : _history)
            {
                gui::Rect snakePart(
                    s.x - _headRadius,
                    s.y - _headRadius,
                    s.x + _headRadius,
                    s.y + _headRadius);

                if (tempRect.intersects(snakePart))
                {
                    validPos = false;
                    break;
                }
            }

            if (validPos)
            {
                _itemRect = tempRect;
                break;
            }
        }
        int attempts = 0;
        while (!validPos && attempts < 100) {
            // tvoja random logika
            attempts++;
        }

        if (!validPos) { // Ako nismo našli idealno, daj bilo šta
            _itemRect = { _size.width / 2.0f, _size.height / 2.0f, _itemSize, _itemSize };
        }
        _itemActive = true;
        this->reDraw();
    }



    void onResize(const gui::Size& newSize) override
    {
        _size = newSize;
        _randomX = std::uniform_int_distribution<int>((int)_gridSize, (int)_size.width - (int)_gridSize * 2);
        _randomY = std::uniform_int_distribution<int>((int)_gridSize, (int)_size.height - (int)_gridSize * 2);
        if (!_gameInProgress && !_gameOver)
        {
            
            newGame();
        }
    }
    

    void moveSnake()
    {
        gui::Point oldPos = _snakePos;
        float moveAmount = _stepSize;

        if (_speedBoostActive) moveAmount *= _speedMultiplier;
        
        switch (_direction) {
        case Direction::RIGHT: _snakePos.x += moveAmount; break;
        case Direction::LEFT:  _snakePos.x -= moveAmount; break;
        case Direction::UP:    _snakePos.y -= moveAmount; break;
        case Direction::DOWN:  _snakePos.y += moveAmount; break;
        default: break;
        }

        float dist = std::sqrt(std::pow(_snakePos.x - oldPos.x, 2) + std::pow(_snakePos.y - oldPos.y, 2));

        int steps = 1;

        for (int i = 1; i <= steps; ++i) {
            float t = (float)i / (float)steps;
            _history.insert(_history.begin(), { oldPos.x + (_snakePos.x - oldPos.x) * t,
                                                oldPos.y + (_snakePos.y - oldPos.y) * t });
        }
        // Historija mora biti usklađena sa iscrtavanjem
        size_t maxHistory = _currentSnakeSize * 4;

        if (_history.size() > maxHistory)
            _history.resize(maxHistory);
    }

    void checkCollision()
    {
        if (_invulnerabilityTimer > 0) {
            _invulnerabilityTimer -= 0.02f; 
            return;
        }
        float margin = 5.0f;
        if (_snakePos.x < 0 || _snakePos.x > _size.width || _snakePos.y < 0 || _snakePos.y > _size.height) {
            // Zidovi i dalje ubijaju odmah (ili možeš i ovdje smanjiti život)
            _lives--;
            if (_lives <= 0) {
                setGameOver();
                return;
            }
            else {
                // Teleport na sredinu ako imaš još života
                _snakePos = { _size.width / 2, _size.height / 2 };
                _history.clear();
                _history.push_back(_snakePos);
                _invulnerabilityTimer = 1.5f; // 1.5 sekunda mira
                return;
            }
        }

        for (size_t i = 30; i < _history.size(); i += 5) {
            float dx = _snakePos.x - _history[i].x;
            float dy = _snakePos.y - _history[i].y;
            float distance = std::sqrt(dx * dx + dy * dy); // Distance formula

            if (distance < _bodyRadius) {
                _lives--;
                if (_pUpdateStatus) (*_pUpdateStatus)((td::INT1)-1, _score, _secondsRemaining);

                if (_lives <= 0) {
                    setGameOver();
                }else
                {
                    _snakePos = { _size.width / 2, _size.height / 2 };
                    _history.clear();
                    _history.push_back(_snakePos);
                    _direction = Direction::RIGHT;
                    _invulnerabilityTimer = 2.0f; // Ključno: daje ti vremena da se snađeš
                    _screenShakeTimer = 0.3f;
                }
                return;
            }
        }

        float hitScale = 0.8f;

        gui::Rect headRect(
            _snakePos.x - _headRadius * hitScale,
            _snakePos.y - _headRadius * hitScale,
            _snakePos.x + _headRadius * hitScale,
            _snakePos.y + _headRadius * hitScale
        );

        if (_itemActive && _itemRect.intersects(headRect))
        {
            handleItem();
        }

    }

    void handleItem()
    {
        ItemType hitType = _currentItem;

        auto spawnParticles = [&]()
            {
                for (int i = 0; i < 6; i++)
                {
                    Particle p;
                    p.pos = _snakePos;

                    float angle = ((float)rand() / RAND_MAX) * 6.28f;
                    float speed = 4.0f;

                    p.velocity = { cos(angle) * speed, sin(angle) * speed };
                    p.life = 0.5f;

                    _particles.push_back(p);
                }
            };

        switch (hitType)
        {
        case ItemType::Apple:
            
            _score += 1;
            _currentSnakeSize += _growthFactor;
            _appleBite.play();
            _screenShakeTimer = 0.1f; // Kratki trzaj ekrana za "impact"
            spawnParticles();
            for (int i = 0; i < 6; i++)
            {
                Particle p;
                p.pos = _snakePos;
                float angle = ((float)rand() / RAND_MAX) * 6.28f;
                float speed = 4.0f;

                p.velocity = { cos(angle) * speed, sin(angle) * speed };
                p.life = 0.5f;
                _particles.push_back(p);
            }
            break;

        case ItemType::GoldenApple:
            _screenShakeTimer = 0.1f; // Kratki trzaj ekrana za "impact"
            _score += 5;
            _currentSnakeSize += 5;
            _sndGoldenApple.play();
            _rainbowMode = true;
            _rainbowTimer = 0.0f;
            spawnParticles();
            for (int i = 0; i < 6; i++)
            {
                Particle p;
                p.pos = _snakePos;
                float angle = ((float)rand() / RAND_MAX) * 6.28f;
                float speed = 4.0f;

                p.velocity = { cos(angle) * speed, sin(angle) * speed };
                p.life = 0.5f;
                _particles.push_back(p);
            }
            break;

        case ItemType::Timer:
            
            _screenShakeTimer = 0.1f; // Kratki trzaj ekrana za "impact"
            _secondsRemaining += 10;
            _sndTimer.play();
            break;

        case ItemType::Speed:
            _speedBoostActive = true;
            _screenShakeTimer = 0.1f; // Kratki trzaj ekrana za "impact"
            _speedBoostTimer = 4.0f;
            _speedMultiplier = 1.5f;
            _sndSpeed.play();
            spawnParticles();
            break;

        case ItemType::Bomb:
            _screenShakeTimer = 0.1f; // Kratki trzaj ekrana za "impact"
            _bomb.play();
            setGameOver();
            return;
        }
        

        if (_pUpdateStatus)
            (*_pUpdateStatus)((td::INT1)hitType, _score, _secondsRemaining);

        _itemActive = false;
        spawnItem();
    }

    td::ColorID lerpColorID(td::ColorID a, td::ColorID b, float t)
    {
        td::BYTE r1, g1, b1, a1;
        td::BYTE r2, g2, b2, a2;

        gui::Color::getRGBA(a, r1, g1, b1, a1);
        gui::Color::getRGBA(b, r2, g2, b2, a2);

        td::BYTE r = (td::BYTE)(r1 + (r2 - r1) * t);
        td::BYTE g = (td::BYTE)(g1 + (g2 - g1) * t);
        td::BYTE bC = (td::BYTE)(b1 + (b2 - b1) * t);
        td::BYTE aC = (td::BYTE)(a1 + (a2 - a1) * t);

        td::Color tdColor(r, g, bC, aC);

        return gui::Color::toColorID(tdColor);
    }
    td::ColorID neonGreen = td::ColorID::LimeGreen;
    td::ColorID blue = td::ColorID::Cyan;
    td::ColorID purple = td::ColorID::Magenta;

    void onDraw(const gui::Rect& drawRect) override
    {
        gui::Rect rect = drawRect;
        
        // --- EKRAN IZBORA MODA (crta se prvi i sam, bez HUD-a) ---
        if (_modeSelection) {
            drawModeSelection();
            return;
        }

        if (_screenShakeTimer > 0)
        {
            float shakeX = (float)((rand() % 6) - 3);
            float shakeY = (float)((rand() % 6) - 3);
            rect.left += shakeX; rect.right += shakeX;
            rect.top += shakeY; rect.bottom += shakeY;
        }

        //_backgroundImg.draw(gui::Rect(0, 0, (gui::CoordType)_size.width, (gui::CoordType)_size.height));
        // Umjesto slike, crtamo pravougaonik preko cijelog ekrana
        //gui::Shape::drawRect(gui::Rect(0, 0, _size.width, _size.height), td::ColorID::Black, td::ColorID::Black, 1.0f);
        //gui::Shape::drawRect(gui::Rect(0, 0, _size.width, _size.height), td::ColorID::LightPink, td::ColorID::LightPink, 1.0f);
        //gui::Shape::drawRect(gui::Rect(0, 0, _size.width, _size.height), td::ColorID::DarkGray, td::ColorID::DarkGray, 1.0f);
        _backgroundImg.draw(gui::Rect(0, 0, (gui::CoordType)_size.width, (gui::CoordType)_size.height));
        
        // Particles
        for (auto& p : _particles)
        {
            gui::Rect pr(p.pos.x - 2, p.pos.y - 2, p.pos.x + 2, p.pos.y + 2);
            gui::Shape::drawRect(pr, td::ColorID::Red, td::ColorID::Orange, 0.8f);
        }

        // --- 1. TVOJE ORIGINALNO PULSIRANJE (VRAĆENO) ---
        if (_gameInProgress && _itemActive)
        {
            float pulse = 1.0f + 0.08f * std::sin(_applePulseTime * 1.5f);
            gui::Rect pulseItem = _itemRect;
            float expansion = (_itemSize * (pulse - 1.0f)) / 2.0f;
            pulseItem.inflate(expansion);

            switch (_currentItem)
            {
            case ItemType::Apple:       _apple.draw(pulseItem); break;
            case ItemType::GoldenApple: _goldenApple.draw(pulseItem); break;
            case ItemType::Timer:       _timerImg.draw(pulseItem); break;
            case ItemType::Bomb:        _bombImg.draw(pulseItem); break;
            case ItemType::Speed:       _speedImg.draw(pulseItem); break;
            }
        }


        // Snake Body
        for (size_t i = 0; i < _history.size(); i += 3) 
        {
            td::ColorID bodyColor;


            // Logika boja (Tvoja originalna)
            if (_gameOver) {
                bodyColor = td::ColorID::Gray;
            }
            else if (_rainbowMode) {
                // Rainbow: Magenta -> Cyan, usporeno i OD GLAVE KA REPU (- sign)
                float wave = (sin(-(_rainbowTimer * 4.0f) + i * 0.05f) + 1.0f) * 0.5f;
                bodyColor = lerpColorID(td::ColorID::Magenta, td::ColorID::Cyan, wave);
            }
            else {
                // Magenta i White: GOLATKO PRETAPAnje, usporeno i OD GLAVE KA REPU (- sign)
                float wave = (sin(-(_applePulseTime * 1.5f) + i * 0.05f) + 1.0f) * 0.5f;
                bodyColor = lerpColorID(td::ColorID::Magenta, td::ColorID::White, wave);
            }

            // --- LOGIKA ZA REP: Smanjuj samo zadnjih par krugova ---
            float currentRadius = _bodyRadius;
            if (i > _history.size() - 20) {
                // Zadnjih 40 tačaka historije (što je otprilike 4 nacrtana kruga jer je korak 10)
                float tailProgress = (float)(i - (_history.size() - 20)) / 20.0f;
                currentRadius = _bodyRadius * (1.0f - tailProgress * 0.2f); // Smanjuje se do 40% debljine
            }
            // CRTANJE: Vraćeno na tvoj stil sa crnim okvirom
            gui::Point currentPoint = _history[i];
            gui::Circle bodyCircle(_history[i], currentRadius);

            gui::Shape sBody;
            //sBody.createCircle(bodyCircle);
            sBody.createCircle(gui::Circle(_history[i], currentRadius));
            sBody.drawFillAndWire(bodyColor, td::ColorID::Black, 0.5f);
        }

        // --- CRTANJE GLAVE ZMIJE (Sa očima) ---
        if (!_history.empty()) {
            gui::Point headPos = _history[0];

            // Glava
            gui::Shape sHead;
            sHead.createCircle(gui::Circle(headPos, _headRadius)); // Popravljeno: gui::Circle(centar, radijus)
            td::ColorID headColor = _gameOver ? td::ColorID::DarkGray : td::ColorID::DeepPink;
            sHead.drawFillAndWire(headColor, td::ColorID::Black, 1.5f);

            // Pomeraj za oči prema napred
            float eyeOffX = 0, eyeOffY = 0;
            if (_direction == Direction::RIGHT) eyeOffX = 5;
            else if (_direction == Direction::LEFT) eyeOffX = -5;
            else if (_direction == Direction::UP) eyeOffY = -5;
            else if (_direction == Direction::DOWN) eyeOffY = 5;

            gui::Shape sEye;

            // Funkcija pomoćnik unutar koda da ne ponavljamo grešku
            auto drawEye = [&](gui::Point p) {
                // 1. Beonjača
                sEye.createCircle(gui::Circle(p, 4.0f));
                sEye.drawFillAndWire(td::ColorID::White, td::ColorID::Black, 0.5f);
                // 2. Zenica
                sEye.createCircle(gui::Circle(p, 2.0f));
                sEye.drawFill(td::ColorID::Black);
                // 3. Odsjaj (bijela tačkica)
                sEye.createCircle(gui::Circle({ p.x - 1.0, p.y - 1.0 }, 0.8f));
                sEye.drawFill(td::ColorID::White);
                };

            // Pozicije za lijevo i desno oko u odnosu na centar glave
            // Ako se zmija kreće gore/dole, oči idu lijevo/desno od centra
            // Ako se kreće lijevo/desno, oči idu gore/dole od centra
            float sideX = (eyeOffY != 0) ? 5 : 0;
            float sideY = (eyeOffX != 0) ? 5 : 0;

            drawEye({ headPos.x + eyeOffX - sideX, headPos.y + eyeOffY - sideY }); // Lijevo oko
            drawEye({ headPos.x + eyeOffX + sideX, headPos.y + eyeOffY + sideY }); // Desno oko
        }
        // 4. CRTAŠ SRCAD (SADA NA KRAJU - DA BUDU NA VRHU)
        for (int i = 0; i < 3; i++) {
            // Pomjereno skroz ulijevo i malo niže da ne bježi s ekrana
            gui::Rect heartRect(20 + i * 45, 20, 55 + i * 45, 55);
            if (i < _lives) {
                _heartFull.draw(heartRect);
            }
            else {
                _heartEmpty.draw(heartRect);
            }
        }

        td::String scoreText;
        scoreText.format("SCORE: %d", _score);

        gui::DrawableString ds(scoreText);

        // SKROZ DESNO (podesi 180 ako treba još desnije)
        gui::Point scorePos(_size.width - 130, 30);

        // Glow efekat
        ds.draw(gui::Point(scorePos.x + 3, scorePos.y + 3),
            gui::Font::ID::SystemBold,
            td::ColorID::Magenta);

        ds.draw(gui::Point(scorePos.x - 2, scorePos.y - 2),
            gui::Font::ID::SystemBold,
            td::ColorID::DeepPink);

        ds.draw(scorePos,
            gui::Font::ID::SystemBold,
            td::ColorID::White);

        //gameover
        if (_gameOver)
        {
            _imgGameOver.draw(gui::Rect(0, 0, (gui::CoordType)_size.width, (gui::CoordType)_size.height));
        }
    }
    
    Direction computeAIMove() {
        int cols = (int)(_size.width / _gridSize);
        int rows = (int)(_size.height / _gridSize);

        if (cols <= 5 || rows <= 5) return _direction;

        std::vector<std::vector<int>> grid(rows, std::vector<int>(cols, 0));
        for (int x = 0; x < cols; ++x) {
            grid[0][x] = 1;               // Top row
            grid[rows - 1][x] = 1;          // Bottom row
        }
        for (int y = 0; y < rows; ++y) {
            grid[y][0] = 1;               // Left column
            grid[y][cols - 1] = 1;          // Right column
        }
        // 1. Mark body as obstacles
        for (size_t i = 10; i < _history.size(); i += 4) {
            int gx = std::clamp(
                (int)((_history[i].x - _gridSize / 2) / _gridSize),
                0, cols - 1
            );

            int gy = std::clamp(
                (int)((_history[i].y - _gridSize / 2) / _gridSize),
                0, rows - 1
            );
            grid[gy][gx] = 1;
        }
        // If there is an active bomb, mark it as obstacle
        if (_itemActive && _currentItem == ItemType::Bomb) {
            int bombCX = (int)((_itemRect.left + _itemSize / 2 - _gridSize / 2) / _gridSize);
            int bombCY = (int)((_itemRect.top + _itemSize / 2 - _gridSize / 2) / _gridSize);

            // Mark surrounding 1-cell radius as obstacle
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    int gx = std::clamp(bombCX + dx, 0, cols - 1);
                    int gy = std::clamp(bombCY + dy, 0, rows - 1);
                    grid[gy][gx] = 1;
                }
            }
        }

        // 2. Current head grid position
        int startX = std::clamp(
            (int)((_snakePos.x - _gridSize / 2) / _gridSize),
            0, cols - 1
        );

        int startY = std::clamp(
            (int)((_snakePos.y - _gridSize / 2) / _gridSize),
            0, rows - 1
        );

        // 3. Goal calculation
        int goalX = startX;
        int goalY = startY;

        if (_itemActive)
        {
            if (_itemActive && _currentItem != ItemType::Bomb)
            {
                float cX = (_itemRect.left + _itemRect.right) / 2.0f;
                float cY = (_itemRect.top + _itemRect.bottom) / 2.0f;
                goalX = std::clamp((int)((cX - _gridSize / 2) / _gridSize), 0, cols - 1);
                goalY = std::clamp((int)((cY - _gridSize / 2) / _gridSize), 0, rows - 1);
            }
            else
            {
                goalX = startX; // stay in place or use safe move
                goalY = startY;
            }
        }

        // 4. Run the A* Solver
        auto path = AISolver::findPath(startX, startY, goalX, goalY, grid);
        if (path.empty()) {
            goalX = startX; goalY = startY; // fallback
        }
        int targetX = startX, targetY = startY;
        if (path.size() >= 2) {
            targetX = path[1].first;
            targetY = path[1].second;
        }
        else {
            auto safe = AISolver::findSafeMove(startX, startY, grid);
            targetX = safe.first;
            targetY = safe.second;
        }

        Direction nextDir = _direction;
        if (targetX > startX)      nextDir = Direction::RIGHT;
        else if (targetX < startX) nextDir = Direction::LEFT;
        else if (targetY > startY) nextDir = Direction::DOWN;
        else if (targetY < startY) nextDir = Direction::UP;

        int nextGX = startX, nextGY = startY;
        if (nextDir == Direction::RIGHT)      nextGX++;
        else if (nextDir == Direction::LEFT)  nextGX--;
        else if (nextDir == Direction::DOWN)  nextGY++;
        else if (nextDir == Direction::UP)    nextGY--;

        if (nextGX < 0 || nextGX >= cols || nextGY < 0 || nextGY >= rows || grid[nextGY][nextGX] == 1) {
            auto safe = AISolver::findSafeMove(startX, startY, grid);
            if (safe.first > startX)      nextDir = Direction::RIGHT;
            else if (safe.first < startX) nextDir = Direction::LEFT;
            else if (safe.second > startY) nextDir = Direction::DOWN;
            else if (safe.second < startY) nextDir = Direction::UP;
        }

        if ((nextDir == Direction::LEFT && _direction == Direction::RIGHT) ||
            (nextDir == Direction::RIGHT && _direction == Direction::LEFT) ||
            (nextDir == Direction::UP && _direction == Direction::DOWN) ||
            (nextDir == Direction::DOWN && _direction == Direction::UP)) {
            return _direction;
        }
        
        return nextDir;
    }
protected:
    void drawModeSelection()
    {
        gui::Rect leftHalf(0, 0, _size.width / 2, _size.height);
        gui::Rect rightHalf(_size.width / 2, 0, _size.width, _size.height);

        // Tamna pozadina preko cijelog ekrana
        gui::Shape::drawRect(gui::Rect(0, 0, _size.width, _size.height),
                             td::ColorID::Black, td::ColorID::Black, 1.0f);

        _imgPlayer.draw(leftHalf);
        _imgAI.draw(rightHalf);

        // Zeleni okvir oko trenutno oznacenog izbora
        gui::Shape selectionShape;
        selectionShape.createRect(_aiMode ? rightHalf : leftHalf);
        selectionShape.drawWire(td::ColorID::LimeGreen, 8.0f);

        // Uputa na dnu
        gui::DrawableString hint(tr("modeHint"));
        hint.draw({ _size.width / 2 - 150, _size.height - 100 },
                  gui::Font::ID::SystemBold, td::ColorID::White);

        drawDifficultyToggle();
    }

    void drawDifficultyToggle() {
        // Tri jednaka polja: EASY | MEDIUM | HARD
        gui::Rect toggleArea(_size.width / 2 - 165, _size.height - 80,
                             _size.width / 2 + 165, _size.height - 30);

        gui::Shape::drawRect(toggleArea, td::ColorID::Black, td::ColorID::Gray, 1.0f);

        gui::CoordType segW = (toggleArea.right - toggleArea.left) / 3;

        const Difficulty levels[3] = { Difficulty::Easy, Difficulty::Medium, Difficulty::Hard };
        const char* labels[3] = { "EASY", "MEDIUM", "HARD" };
        const td::ColorID activeColors[3] = { td::ColorID::LimeGreen, td::ColorID::Orange, td::ColorID::Red };

        for (int i = 0; i < 3; i++)
        {
            gui::Rect segRect(toggleArea.left + i * segW, toggleArea.top,
                              toggleArea.left + (i + 1) * segW, toggleArea.bottom);

            bool active = (_difficulty == levels[i]);

            gui::Shape::drawRect(segRect,
                                 active ? activeColors[i] : td::ColorID::DarkGray,
                                 td::ColorID::White,
                                 active ? 2.0f : 0.5f);

            gui::DrawableString(labels[i]).draw(
                { segRect.left + 12, segRect.top + 10 },
                active ? gui::Font::ID::SystemBold : gui::Font::ID::SystemNormal,
                td::ColorID::White);
        }
    }

    bool onKeyPressed(const gui::Key& key) override
    {
        auto vKey = key.getVirtual();
        
        // 1. If Game Over, any key restarts to Mode Selection
        if (_gameOver) {
            _gameOver = false;
            _modeSelection = true;
            this->reDraw();
            return true;
        }

        // 2. Mode Selection Logic
        if (_modeSelection) {
            // strelice samo pomjeraju zeleni okvir
            if (vKey == gui::Key::Virtual::Left)  _aiMode = false;
            if (vKey == gui::Key::Virtual::Right) _aiMode = true;

            // 1/2/3 mijenjaju tezinu jos prije pocetka igre
            char cSel = key.getChar();
            if (cSel == '1') setDifficulty(Difficulty::Easy);
            if (cSel == '2') setDifficulty(Difficulty::Medium);
            if (cSel == '3') setDifficulty(Difficulty::Hard);

            // Enter potvrdjuje izbor i pokrece igru
            if (vKey == gui::Key::Virtual::Enter || key.isEnter()) {
                newGame();
                _modeSelection = false;
                if (_pUpdateUI) (*_pUpdateUI)();          // meni/toolbar prate izabrani mod
                if (_pStartGame) (*_pStartGame)();        // MainView pokrece tajmer
            }
            this->reDraw();
            return true;
        }

        // 3. Difficulty Shortcuts (During Gameplay)
        char c = key.getChar();
        if (c == '1') setDifficulty(Difficulty::Easy);
        if (c == '2') setDifficulty(Difficulty::Medium);
        if (c == '3') setDifficulty(Difficulty::Hard);

        // 4. Manual Movement (Only if AI is OFF)
        if (!_aiMode) {
            switch (vKey) {
            case gui::Key::Virtual::Up:    if (_direction != Direction::DOWN) _direction = Direction::UP; break;
            case gui::Key::Virtual::Down:  if (_direction != Direction::UP)   _direction = Direction::DOWN; break;
            case gui::Key::Virtual::Left:  if (_direction != Direction::RIGHT) _direction = Direction::LEFT; break;
            case gui::Key::Virtual::Right: if (_direction != Direction::LEFT)  _direction = Direction::RIGHT; break;
            default: break;
            }
        }

        this->reDraw();
        return true;
    }
};

# main
## main.cpp
Просто обёртка для demos.h, в которой указывается номер демонстрации.
## demos.h
Набор из 6 демо:
0. Пример нажатия кнопок, масштабирования элементов ui. Переключение сцен.
1. Карта тайлов, выбор тайла, вывод текста.
2. Пример текстуры с фрагментным шейдером.
3. Текстура из 9 частей (для ровных углов, как у кнопки), первое применение парсера форм.
4. текстура кнопки из демо №3 отдельно.
5. Заготовка для сцены "человек vs бот". второе Изображение показывает состояние игры - 
оранжевая текстура означает, что ходит игрок, а фиолетовая - что идёт ожидание хода бота. "Игра" представляет 
из себя следующее: игроки нажатием на клетку доски меняют её на случайную другую. Смысла, естественно, нет.
6. Иллюстрация преломления луча через стеклянный шарик. 

# chess
Реализация логики шахмат без графики. В процессе.
- **Board**: Сама шахматная доска; хранит состояния клеток, флаги фигур ("двигался ли король") и историю ходов. История ходов больше пригодится не для просмотра партии, а для более быстрой работы minmax-а.
- **Figure**: = uint8_t; первые три бита под цвет, оставшиеся 5 - под вид фигуры.
- **Step & Step::Part**: ход фигуры, разделён на несколько частей, т.к. в шахматах есть составные ходы: рокировка, продвижение пешки и взятие на проходе. Step::Part представляет из себя  ████████ ██████ и ██████ █████.

# connection
Заголовники, реализующие объекты клиента и сервера, которые являются обёрткой над сокетами из sfml. Пока не используются в демо и, 
скорее всего, будут удалены.

# pinboard
"Игра" для демо №5.

# scecials
Необходимые примитивные методы и классы.
## Box2.h/cpp
- **Box2** - удобная версия sf::Rect.
## specials.h/cpp
- **Valid**: Проверка того, что целочисленная точка является координатой существующей клетки на доске размера X * Y.

# ui
## Window.h/cpp & AppManager.cpp
Объекты, отвечающие за внешний вид окна и взаимодействие с ним. 
- **Window** - окно, содержит в себе массив ссылок на ui::Element, которые образуют интерфейс.
   - **StartRenderCycle** - запуск окна после установки всех сцен. Вызывает в отдельном потоке RenderCycle.
   - **RenderCycle** - цикл, рендера, как ни странно. Выполняется пока окно не пришло в состояние END или ERROR. Т.к. деструктор окна содержит ожидание.
      потока, в котором этот метод вызван, окно н закрывается сразу после запуска.
   - **SetManager** - метод связи окна и обработчика событий.
- **AppManager** - обработчик событий, посылаемых window. Хранит обработки событий, как лямбды. Так сделано, т.к.
обработчик должен захватывать данные приложения, поэтому пользоваться обычным наследованием от класса AppManager было бы неудобно.
Эти методы также принимают по указателю сам менеджер, чтобы можно было вызывать методы закрытия окна, смены сцены и разблокировки кнопок (после нажатия кнопки и проигрыша "анимации" нажатия окно блокирует нажатия до тех пор, пока менеджер не обработает последнее).
    - **obc** (OnButtonClick) - обработка нажатия кнопки. Принимает индекс кнопки и координату нажатия. С помощью метода ToUnit можно привести его к диапазону [0; 1] X [0; 1], что и используется для обработки нажатия на сетку тайлов.
    - **oe** (OnExit) - обработка закрытия окна. Т.к. менеджер хранит индекс текущей сцены, можно по-разному обрабатывать закрытие в разных ситуациях.
    - **oss** (OnSceneSwitch) - обработка открытия сцены.
    - **oke** (OnKeyEvent) - обработка нажатия кнопки. Также поддерживает комбинации типа shift+A.

## sprites.cpp
реализация графических примитивов (т.е. спрайтов и блоков текста)
- **Sprite** - спрайт, может быть 3х типов: Пустой, из 9 частей и одиночный. 
    - **Base** - основа спрайта, заранее загруженный sf::Texture и sf::Sprite.
    - **Param** - аргументы конструктора спрайта. Т.к. их довольно много, пришлось выделить в отдельный тип.
    - **Draw** - метод, который рисует спрайт на окне в указанной зоне, полностью растягивая.
- **Text**
  - **Draw** - аналогично Sprite::Draw
  - **SetString** - установка нового текста.
- **ShaderSprite** - класс спрайта с шейдером. Код шейдера загружается на этапе выполнения.
  - **SetUniform** - установка uniform-значения в шейдере. Автоматически устанавливаются значения u_bl и u_tr - координаты левой нижней и правой верхней вершины текстуры после трансформации.

## uibasic.cpp
Реализация родительского класса ui::Element и дочерних обёрток для примитивов.
- **IsInside** - (проверка того, что точка лежит внутри элемента ui).
- **ToUnit** - Масштабирование координаты, писал выше.
- **Is/Set** - Установка и чтение флага. Возможно, следует убрать и использовать битовые поля.
- **Reshape** - в зависимости от типа масштабирования, оригинальной и изменённой областей родительского элемента и оригинальной области себя, растягивает область элемента.
Доступны варианты: прикрепить к центру (bind__), растянуть на всю родительскую область (fullZone), просто расстянуть (scaleXY), растянуть сохранив пропорции и оставить в центре (fitCenter). 
- **OnMouseEvent** - обработка нажатия мыши (3 типа состояния - зажимается, удерживается и отпускается).
- **Draw** - отрисовка элемента.
## uigroups.cpp & uibutton.cpp & uitilemaps.cpp
Реализация дочерних классов ui::Element
- **Group** - группа элементов. 
  - **Variant** - группа, но виден одновременно только один подэлемент.  
  - **SideCut** - группа из 3х элементов (или менее), которые делят область на три части так же, как делит выравнивание fitCenter
  - **Grid** - группа, масштабирующая дочерние элементы, как элементы сетки.
- **Tilemap** - карта тайлов.
- **SelectionTM** - выделение на одном из тайлов на карте тайлов.
- **Button & InvisibleButton** - кнопки.
## formparser.h/cpp
Реализация чтения формы сцены из файла. В разработке.

Элементы создаются так:
```
<type> <name> <zone> <scale mode> <additional>
# например, так:
image img0 : 100 200 500 600 bindTR ...
```
В зависимости от типа объекта (тип не чуствителен к регистру, к слову), требуются разлиные доп параметры.
```
image img0 : 100 200 500 600 bindTR "sauce.png" 16 16 83 SCALE9
# <source> = <string> <X, Y, I, T> 
# image ... <source>
# в качестве string - имя файла
# X, Y - метрика текстурного атласа
# I - индекс первой подтекстуры
# T - тип (см. Sprite)
```
```
Group scene0 : 0 0 1000 800 bindTL
    <element 1>
    <element 2>
    ...
    <element 3>
@ 
# <element i> - описание подэлемента группы.
# Переносы и табуляции не играют роли, необходимо просто наличие пробельного символа
```
```
TileMap tm : 0 0 1 1 fullZone "chesstilemap.png" : 16 16 0 -1
            sel "src4.png" 1 1 0

# TileMap ... <src> <X, Y, I, J>
#             sel <source>
# src, X, Y - см. Image. I - начало отрезка загружаемых текстур, J - конец. 
# Если J = -1, то J := X * Y - 1
# sel - это выделение 
```
```
# button ... <source> <source> <ID>
# inisiblebutton ... <ID>
# первый source - текстура отжатой кнопки, второй - зажатой.
# ID - id, который получает менеджер окна.
```
```
text ... <string> <string>
# первая строка - текст при запуске, вторая - имя файла шрифта. 
```
Также, в некоторых местах можно поставить параметры по умолчанию с помощью символа @:
```
<zone> --> : 0 0 1 1
<scaleMode> --> fullZone
<X, Y, I> --> 1 1 0
# также, если поставить @ вместо sel ..., то выделение создаваться не будет.
```
@ Также обозначает и конец файла:
```
#пока нет поддержки комментариев, но если будут, то такие

Group scene0 : 0 0 1000 800 bindTL
    Group grid : 0 100 700 800 bindTL
    TileMap tm @ fullZone "chesstilemap.png" 16 16 0 -1 @ # первая собака - 
        InvisibleButton btn @ fullZone 0
    @
    Text evalText : 0 0 700 100 bindTL "Eval:....." "Ninja Naruto.ttf"
    Button evalButton : 800 0 1000 100 bindTL "chesstilemap.png" : 16 16 32 scale9 100 100 "src1.png" @ 1
    Group varGrid : 800 100 1000 800 bindTL
        TileMap tm @ fullZone "chesstilemap.png" 16 16 0 -1
            sel "src4.png" @    # вот к этому элементу мы и получим доступ.
        InvisibleButton btn @ fullZone 2
    @
@           # эта собака - конец группы
@           # а эта - конец файла
```
После чтения из файла:
```c++
 parser.Parse("demo1", mainWin); 
```
Можно получить доступ к элементам следующим образом:
```c++
auto varGrid_sel = (SelectionTM*)parser["scene0_varGrid_tm_sel"];
```

















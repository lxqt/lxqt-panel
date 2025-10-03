<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="bg">
<context>
    <name>ColorLabel</name>
    <message>
        <location filename="../colorLabel.cpp" line="36"/>
        <source>Click to change color</source>
        <translation>Кликнете, за да промените цвета</translation>
    </message>
    <message>
        <location filename="../colorLabel.cpp" line="65"/>
        <source>Select Color</source>
        <translation>Изберете цвят</translation>
    </message>
</context>
<context>
    <name>LXQtCustomCommandConfiguration</name>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="14"/>
        <source>Custom Command Configuration</source>
        <translation>Конфигуриране на персонализирана команда</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="20"/>
        <source>Display</source>
        <translation>Дисплей</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="222"/>
        <source>Use icon from theme or browse file</source>
        <translation>Използвайте икона от темата или изберете файл</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="104"/>
        <source>Run with &quot;bash -c&quot;</source>
        <translation>Изпълняване с &quot;bash -c&quot;</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="42"/>
        <source>Select Font</source>
        <translation>Избиране на шрифт</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="239"/>
        <source>Text</source>
        <translation>Текст</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="72"/>
        <source>Command</source>
        <translation>Команда(Инструкция)</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="35"/>
        <source>Font</source>
        <translation>Шрифт</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="279"/>
        <source> px</source>
        <translation> пиксела</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="249"/>
        <source>%1</source>
        <translation>%1</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="252"/>
        <source>Use %1 to display command output</source>
        <translation>Използвайте %1, за да покажете изходът от командата</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="212"/>
        <source>Icon</source>
        <translation>Икона</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="229"/>
        <source>Browse</source>
        <translation>Разглеждане</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="28"/>
        <source>Autorotate when the panel is vertical</source>
        <translation>Автоматично завъртане когато панелът е в вертикално положение</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="94"/>
        <source>echo Configure...</source>
        <translation>echo Конфигуриране...</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="97"/>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="319"/>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="336"/>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="353"/>
        <source>Command to run</source>
        <translation>Команда за изпълнение</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="196"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Text only&lt;/span&gt; - command outputs plain text to be used as text of the button&lt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Icon only&lt;/span&gt; - command outputs icon in form of:&lt;br/&gt;&amp;nbsp;&amp;nbsp;- &lt;span style=&quot; font-style:italic;&quot;&gt;theme name&lt;/span&gt; - name of icon resolved to image based on XDG spec&lt;br/&gt;&amp;nbsp;&amp;nbsp;- &lt;span style=&quot; font-style:italic;&quot;&gt;image file path&lt;/span&gt; - path to file with image to show&lt;br/&gt;&amp;nbsp;&amp;nbsp;- &lt;span style=&quot; font-style:italic;&quot;&gt;image data stream&lt;/span&gt; - plain image stream&lt;br/&gt;&amp;nbsp;&amp;nbsp;- &lt;span style=&quot; font-style:italic;&quot;&gt;base64 encoded image data stream&lt;/span&gt; - as above but base64 encoded&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Stuctured&lt;/span&gt; - command outputs structured variables to be used for button visualization in form &amp;quot;&lt;span style=&quot; text-decoration: underline;&quot;&gt;name1:base64value1 name2:base64value2 ...&lt;/span&gt;&amp;quot;. Handled names are:&lt;br/&gt;&amp;nbsp;&amp;nbsp;- &lt;span style=&quot; font-style:italic;&quot;&gt;text&lt;/span&gt; - string to be used as text of the button&lt;br/&gt;&amp;nbsp;&amp;nbsp;- &lt;span style=&quot; font-style:italic;&quot;&gt;icon&lt;/span&gt; - icon to be show in the button, in the same form as in &lt;span style=&quot; font-weight:700;&quot;&gt;Icon only&lt;/span&gt; output&lt;br/&gt;&amp;nbsp;&amp;nbsp;- &lt;span style=&quot; font-style:italic;&quot;&gt;tooltip&lt;/span&gt; - string to be used as tooltip of the button&lt;br/&gt;Example of script generating structured output:&lt;br/&gt;&lt;span style=&quot; font-style:italic;&quot;&gt;echo &amp;quot;text:$(echo -n &amp;quot;My text&amp;quot; | base64 --wrap=0) icon:$(base64 --wrap=0 my_image.svg) tooltip:$(echo -n &amp;quot;This is my pretty tooltip&amp;quot; | base64 --wrap=0)&amp;quot;&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="199"/>
        <source>Output format:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="269"/>
        <source>Max Width</source>
        <translation>Максимална широчина</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="156"/>
        <source>Repeat command after:</source>
        <translation>Повтаряне на команда след:</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="52"/>
        <source>Text Color</source>
        <translation>Цвят на текста</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="62"/>
        <source>Reset</source>
        <translation>Нулиране</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="172"/>
        <source> second(s)</source>
        <translation> секунда(и)</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="259"/>
        <source>Tooltip</source>
        <translation>Съвет</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="303"/>
        <source>Mouse Commands</source>
        <translation>Команди за мишката</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="309"/>
        <source>Click</source>
        <translation>Кликване</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="326"/>
        <source>Wheel Up</source>
        <translation>Колело нагоре</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.ui" line="343"/>
        <source>Wheel Down</source>
        <translation>Колело надолу</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.cpp" line="38"/>
        <source>Text only</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.cpp" line="39"/>
        <source>Icon only</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.cpp" line="40"/>
        <source>Structured</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.cpp" line="190"/>
        <source>Select Icon File</source>
        <translation>Избиране на файл с икона</translation>
    </message>
    <message>
        <location filename="../lxqtcustomcommandconfiguration.cpp" line="190"/>
        <source>Images (*.png *.svg *.xpm *.jpg)</source>
        <translation>Изображения (*.png *.svg *.xpm *.jpg)</translation>
    </message>
</context>
</TS>

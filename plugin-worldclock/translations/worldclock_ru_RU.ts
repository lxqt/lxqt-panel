<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru_RU">
<context>
    <name>LxQtWorldClock</name>
    <message>
        <location filename="../lxqtworldclock.cpp" line="187"/>
        <source>&apos;&lt;b&gt;&apos;HH:mm:ss&apos;&lt;/b&gt;&lt;br/&gt;&lt;font size=&quot;-2&quot;&gt;&apos;ddd, d MMM yyyy&apos;&lt;br/&gt;&apos;TT&apos;&lt;/font&gt;&apos;</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>LxQtWorldClockConfiguration</name>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="14"/>
        <source>World Clock Settings</source>
        <translation>Настройки мирового времени</translation>
    </message>
    <message>
        <source>&amp;Short, time only</source>
        <translation type="vanished">&amp;Короткий, только время</translation>
    </message>
    <message>
        <source>&amp;Long, time only</source>
        <translation type="vanished">&amp;Длинный, только время</translation>
    </message>
    <message>
        <source>S&amp;hort, date &amp;&amp; time</source>
        <translation type="vanished">К&amp;ороткий, дата и время</translation>
    </message>
    <message>
        <source>L&amp;ong, date &amp;&amp; time</source>
        <translation type="vanished">Д&amp;линный, дата и время</translation>
    </message>
    <message>
        <source>&amp;Custom</source>
        <translation type="vanished">&amp;Свой</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;Custom Date/Time Format Syntax&lt;/span&gt;&lt;/p&gt;&lt;p&gt;A date pattern is a string of characters, where specific strings of characters are replaced with date and time data from a calendar when formatting or used to generate data for a calendar when parsing.&lt;/p&gt;&lt;p&gt;The Date Field Symbol Table below contains the characters used in patterns to show the appropriate formats for a given locale, such as yyyy for the year. Characters may be used multiple times. For example, if y is used for the year, &apos;yy&apos; might produce &apos;99&apos;, whereas &apos;yyyy&apos; produces &apos;1999&apos;. For most numerical fields, the number of characters specifies the field width. For example, if h is the hour, &apos;h&apos; might produce &apos;5&apos;, but &apos;hh&apos; produces &apos;05&apos;. For some characters, the count specifies whether an abbreviated or full form should be used, but may have other choices, as given below.&lt;/p&gt;&lt;p&gt;Two single quotes represents a literal single quote, either inside or outside single quotes. Text within single quotes is not interpreted in any way (except for two adjacent single quotes). Otherwise all ASCII letter from a to z and A to Z are reserved as syntax characters, and require quoting if they are to represent literal characters. In addition, certain ASCII punctuation characters may become variable in the future (eg &amp;quot;:&amp;quot; being interpreted as the time separator and &apos;/&apos; as a date separator, and replaced by respective locale-sensitive characters in display).&lt;br/&gt;&lt;/p&gt;&lt;table border=&quot;1&quot; style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;&quot; width=&quot;100%&quot; cellspacing=&quot;0&quot; cellpadding=&quot;4&quot;&gt;&lt;tr&gt;&lt;td width=&quot;20%&quot;&gt;&lt;p align=&quot;center&quot;&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Code&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p align=&quot;center&quot;&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Meaning&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;d&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the day as number without a leading zero (1 to 31)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;dd&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the day as number with a leading zero (01 to 31)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;ddd&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the abbreviated localized day name (e.g. &apos;Mon&apos; to &apos;Sun&apos;).&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;dddd&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the long localized day name (e.g. &apos;Monday&apos; to &apos;Sunday&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;M&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the month as number without a leading zero (1-12)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;MM&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the month as number with a leading zero (01-12)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;MMM&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the abbreviated localized month name (e.g. &apos;Jan&apos; to &apos;Dec&apos;).&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;MMMM&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the long localized month name (e.g. &apos;January&apos; to &apos;December&apos;).&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;yy&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the year as two digit number (00-99)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;yyyy&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the year as four digit number&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;h&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;hh&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;H&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the hour without a leading zero (0 to 23, even with AM/PM display)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;HH&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the hour with a leading zero (00 to 23, even with AM/PM display)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;m&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the minute without a leading zero (0 to 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;mm&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the minute with a leading zero (00 to 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;s&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the second without a leading zero (0 to 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;ss&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the second with a leading zero (00 to 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;z&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the milliseconds without leading zeroes (0 to 999)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;zzz&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the milliseconds with leading zeroes (000 to 999)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;AP or A&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;use AM/PM display. &lt;span style=&quot; font-weight:600;&quot;&gt;A/AP&lt;/span&gt; will be replaced by either &amp;quot;AM&amp;quot; or &amp;quot;PM&amp;quot;.&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;ap or a&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;use am/pm display. &lt;span style=&quot; font-weight:600;&quot;&gt;a/ap&lt;/span&gt; will be replaced by either &amp;quot;am&amp;quot; or &amp;quot;pm&amp;quot;.&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;t&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the timezone (for example &amp;quot;CEST&amp;quot;)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;T&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the offset from UTC&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;TT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the timezone IANA id&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;TTT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the timezone abbreviation&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;TTTT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the timezone short display name&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;TTTTT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the timezone long display name&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;p&gt;&lt;br/&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Note:&lt;/span&gt; Any characters in the pattern that are not in the ranges of [&apos;a&apos;..&apos;z&apos;] and [&apos;A&apos;..&apos;Z&apos;] will be treated as quoted text. For instance, characters like &apos;:&apos;, &apos;.&apos;, &apos; &apos;, &apos;#&apos; and &apos;@&apos; will appear in the resulting time text even they are not enclosed within single quotes.The single quote is used to &apos;escape&apos; letters. Two single quotes in a row, whether inside or outside a quoted sequence, represent a &apos;real&apos; single quote.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;Синтаксис своего формата даты/времени&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Шаблон даты — это строка символов, где специальные группы символов заменяются при форматировании на дату и время из календаря или используются для генерации даты при синтаксическом разборе.&lt;/p&gt;&lt;p&gt;Таблица символов полей даты ниже содержит символы, используемые в шаблонах для отображения подходящих форматов установленной локали, такой как yyyy для года. Символы могут быть использованы несколько раз. Например, если использован y для года, &apos;yy&apos; отобразит &apos;99&apos;, тогда как &apos;yyyy&apos; покажет &apos;1999&apos;. Для большинства числовых полей число символов определяет ширину поля. Например, если h это час, &apos;h&apos; отобразит &apos;5&apos;, но &apos;hh&apos; отобразит &apos;05&apos;. Для некоторых символов, это число определяет должно ли быть использовано сокращение или полная форма, но могут быть и другие варианты, как написано ниже.&lt;/p&gt;&lt;p&gt;Две одинарные кавычки отобразят одиночную кавычку, независимо от того внутренние они или внешние. Текст внутри одинарных кавычек никак не интерпретируется (за исключением двух смежных одинарных кавычек). Тогда как все символы ASCII от a до z и A до Z зарезервированы под символы синтаксиса, и требуют заключения в кавычки для отображения их как обычных символов. помимо прочего, некоторые знаки препинания ASCII могут стать переменными в будущем (т.е. &amp;quot;:&amp;quot; быть интерпретированы как разделитель компонентов времени и &apos;/&apos; как разделитель компонентов даты, и заменены при отображении соответствующими символами с учётом локали ).&lt;br /&gt;&lt;/p&gt;&lt;table border=&quot;1&quot; style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;&quot; width=&quot;100%&quot; cellspacing=&quot;0&quot; cellpadding=&quot;4&quot;&gt;&lt;tr&gt;&lt;td width=&quot;20%&quot;&gt;&lt;p align=&quot;center&quot;&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Код&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p align=&quot;center&quot;&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Обозначения&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;d&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;день как число без первого нуля (от 1 до 31)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;dd&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;день как число с первым нулём (от 01 до 31)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;ddd&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;сокращённое локализованное название дня (т.е. от &apos;Пн&apos; до &apos;Вс&apos;).&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;dddd&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;длинное локализованное название дня (т.е. от &apos;Понедельник&apos; до &apos;Воскресенье&apos;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;M&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;месяц как число без первого нуля (1-12)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;MM&lt;/td&gt;&lt;td&gt;&lt;p&gt;месяц как число с первым нулём (01-12)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;MMM&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;сокращённое локализованное название месяца (т.е. от &apos;Янв&apos; до &apos;Дек&apos;).&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;MMMM&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;длинное локализованное название месяца (т.е. от &apos;Январь&apos; до &apos;Декабрь&apos;).&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;yy&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;год как двухразрядное число (00-99)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;yyyy&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;год как четырёхразрядное число&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;h&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;час без нуля впереди (от 0 до 23 или 1 до 12, если отображается AM/PM)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;hh&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;час с нулём впереди (от 00 до 23 или 01 до 12, если отображается AM/PM)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;H&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;час без нуля впереди (от 00 до 23, даже с отображением AM/PM)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;HH&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;час с нулём впереди (от 00 до 23, даже с отображением AM/PM)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;m&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;минута без нуля впереди (от 0 до 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;mm&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;минута с нулём впереди (от 00 до 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;s&lt;/td&gt;&lt;td&gt;&lt;p&gt;секунда без нуля впереди (от 0 до 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;ss&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;секунда с нулём впереди (от 00 до 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;z&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;миллисекунда без нуля впереди (от 0 до 999)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;zzz&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;миллисекунда с нулём впереди (от 000 до 999)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;AP или A&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;использовать отображение AM/PM. &lt;span style=&quot; font-weight:600;&quot;&gt;A/AP&lt;/span&gt; будет замещено или &amp;quot;AM&amp;quot; или &amp;quot;PM&amp;quot;.&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;ap или a&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;использовать отображение am/pm. &lt;span style=&quot; font-weight:600;&quot;&gt;a/ap&lt;/span&gt; будет замещено или &amp;quot;am&amp;quot; или &amp;quot;pm&amp;quot;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;t&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;часовой пояс (например &amp;quot;CEST&amp;quot;)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;T&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;сдвиг времени от UTC&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;TT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;id часового пояса IANA&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;TTT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;аббревиатура часового пояса&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;TTTT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;короткое имя часового пояса&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;TTTTT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;длинное имя часового пояса&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;p&gt;&lt;br/&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Замечание:&lt;/span&gt; Любой символ в шаблоне не из диапазона [&apos;a&apos;..&apos;z&apos;] и [&apos;A&apos;..&apos;Z&apos;] будет обработан как цитируемый текст. Например, символы как &apos;:&apos;, &apos;.&apos;, &apos; &apos;, &apos;#&apos; и &apos;@&apos; появятся в тексте со временем, даже если они не помещены в одинарные кавычки. Одинарные кавычки используются для управляющих символ. Две одинарные кавычки подряд, независимо от того внутри или снаружи цитируемой последовательности, представляет &apos;реальные&apos; одинарные кавычки.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="403"/>
        <source>Time &amp;zones</source>
        <translation>Часовые &amp;пояса</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="446"/>
        <source>&amp;Add ...</source>
        <translation>&amp;Добавить…</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="456"/>
        <source>&amp;Remove</source>
        <translation>&amp;Удалить</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="466"/>
        <source>Set as &amp;default</source>
        <translation>Установить &amp;по-умолчанию</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="486"/>
        <source>Move &amp;up</source>
        <translation>&amp;Выше</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="496"/>
        <source>Move do&amp;wn</source>
        <translation>&amp;Ниже</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="24"/>
        <source>Display &amp;format</source>
        <translation>Формат &amp;отображения</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="30"/>
        <source>&amp;Time</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="39"/>
        <source>F&amp;ormat:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="50"/>
        <location filename="../lxqtworldclockconfiguration.ui" line="176"/>
        <location filename="../lxqtworldclockconfiguration.ui" line="272"/>
        <source>Short</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="55"/>
        <location filename="../lxqtworldclockconfiguration.ui" line="181"/>
        <location filename="../lxqtworldclockconfiguration.ui" line="277"/>
        <source>Long</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="60"/>
        <location filename="../lxqtworldclockconfiguration.ui" line="287"/>
        <source>Custom</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="89"/>
        <source>Sho&amp;w seconds</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="96"/>
        <source>Pad &amp;hour with zero</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="116"/>
        <source>T&amp;ime zone</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="128"/>
        <source>&amp;Position:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="138"/>
        <source>For&amp;mat:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="149"/>
        <location filename="../lxqtworldclockconfiguration.ui" line="238"/>
        <source>Below</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="154"/>
        <location filename="../lxqtworldclockconfiguration.ui" line="243"/>
        <source>Above</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="159"/>
        <location filename="../lxqtworldclockconfiguration.ui" line="248"/>
        <source>Before</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="164"/>
        <location filename="../lxqtworldclockconfiguration.ui" line="253"/>
        <source>After</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="186"/>
        <source>Offset from UTC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="191"/>
        <source>Abbreviation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="431"/>
        <source>IANA id</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="201"/>
        <location filename="../lxqtworldclockconfiguration.ui" line="436"/>
        <location filename="../lxqtworldclockconfiguration.cpp" line="591"/>
        <source>Custom name</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="103"/>
        <source>&amp;Use 12-hour format</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="196"/>
        <source>Location identifier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="212"/>
        <source>&amp;Date</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="227"/>
        <source>Po&amp;sition:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="261"/>
        <source>Fo&amp;rmat:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="282"/>
        <source>ISO 8601</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="316"/>
        <source>Show &amp;year</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="323"/>
        <source>Show day of wee&amp;k</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="330"/>
        <source>Pad d&amp;ay with zero</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="337"/>
        <source>&amp;Long month and day of week names</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="350"/>
        <source>Ad&amp;vanced manual format</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="377"/>
        <source>&amp;Customise ...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="476"/>
        <source>&amp;Edit custom name ...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="519"/>
        <source>&amp;General</source>
        <translation>&amp;Общие</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="525"/>
        <source>Auto&amp;rotate when the panel is vertical</source>
        <translation>Авто&amp;поворот для вертикальной панели</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.cpp" line="105"/>
        <source>&apos;&lt;b&gt;&apos;HH:mm:ss&apos;&lt;/b&gt;&lt;br/&gt;&lt;font size=&quot;-2&quot;&gt;&apos;ddd, d MMM yyyy&apos;&lt;br/&gt;&apos;TT&apos;&lt;/font&gt;&apos;</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.cpp" line="591"/>
        <source>Input custom time zone name</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>LxQtWorldClockConfigurationManualFormat</name>
    <message>
        <location filename="../lxqtworldclockconfigurationmanualformat.ui" line="14"/>
        <source>World Clock Time Zones</source>
        <translation type="unfinished">Часовые пояса мирового времени</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfigurationmanualformat.ui" line="83"/>
        <source>&lt;h1&gt;Custom Date/Time Format Syntax&lt;/h1&gt;
&lt;p&gt;A date pattern is a string of characters, where specific strings of characters are replaced with date and time data from a calendar when formatting or used to generate data for a calendar when parsing.&lt;/p&gt;
&lt;p&gt;The Date Field Symbol Table below contains the characters used in patterns to show the appropriate formats for a given locale, such as yyyy for the year. Characters may be used multiple times. For example, if y is used for the year, &apos;yy&apos; might produce &apos;99&apos;, whereas &apos;yyyy&apos; produces &apos;1999&apos;. For most numerical fields, the number of characters specifies the field width. For example, if h is the hour, &apos;h&apos; might produce &apos;5&apos;, but &apos;hh&apos; produces &apos;05&apos;. For some characters, the count specifies whether an abbreviated or full form should be used, but may have other choices, as given below.&lt;/p&gt;
&lt;p&gt;Two single quotes represents a literal single quote, either inside or outside single quotes. Text within single quotes is not interpreted in any way (except for two adjacent single quotes). Otherwise all ASCII letter from a to z and A to Z are reserved as syntax characters, and require quoting if they are to represent literal characters. In addition, certain ASCII punctuation characters may become variable in the future (eg &quot;:&quot; being interpreted as the time separator and &apos;/&apos; as a date separator, and replaced by respective locale-sensitive characters in display).&lt;br /&gt;&lt;/p&gt;
&lt;table border=&quot;1&quot; width=&quot;100%&quot; cellpadding=&quot;4&quot; cellspacing=&quot;0&quot;&gt;
&lt;tr&gt;&lt;th width=&quot;20%&quot;&gt;Code&lt;/th&gt;&lt;th&gt;Meaning&lt;/th&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;d&lt;/td&gt;&lt;td&gt;the day as number without a leading zero (1 to 31)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;dd&lt;/td&gt;&lt;td&gt;the day as number with a leading zero (01 to 31)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;ddd&lt;/td&gt;&lt;td&gt;the abbreviated localized day name (e.g. &apos;Mon&apos; to &apos;Sun&apos;).&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;dddd&lt;/td&gt;&lt;td&gt;the long localized day name (e.g. &apos;Monday&apos; to &apos;Sunday&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;M&lt;/td&gt;&lt;td&gt;the month as number without a leading zero (1-12)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;MM&lt;/td&gt;&lt;td&gt;the month as number with a leading zero (01-12)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;MMM&lt;/td&gt;&lt;td&gt;the abbreviated localized month name (e.g. &apos;Jan&apos; to &apos;Dec&apos;).&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;MMMM&lt;/td&gt;&lt;td&gt;the long localized month name (e.g. &apos;January&apos; to &apos;December&apos;).&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;yy&lt;/td&gt;&lt;td&gt;the year as two digit number (00-99)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;yyyy&lt;/td&gt;&lt;td&gt;the year as four digit number&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;h&lt;/td&gt;&lt;td&gt;the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;hh&lt;/td&gt;&lt;td&gt;the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;H&lt;/td&gt;&lt;td&gt;the hour without a leading zero (0 to 23, even with AM/PM display)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;HH&lt;/td&gt;&lt;td&gt;the hour with a leading zero (00 to 23, even with AM/PM display)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;m&lt;/td&gt;&lt;td&gt;the minute without a leading zero (0 to 59)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;mm&lt;/td&gt;&lt;td&gt;the minute with a leading zero (00 to 59)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;s&lt;/td&gt;&lt;td&gt;the second without a leading zero (0 to 59)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;ss&lt;/td&gt;&lt;td&gt;the second with a leading zero (00 to 59)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;z&lt;/td&gt;&lt;td&gt;the milliseconds without leading zeroes (0 to 999)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;zzz&lt;/td&gt;&lt;td&gt;the milliseconds with leading zeroes (000 to 999)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;AP &lt;i&gt;or&lt;/i&gt; A&lt;/td&gt;&lt;td&gt;use AM/PM display. &lt;b&gt;A/AP&lt;/b&gt; will be replaced by either &quot;AM&quot; or &quot;PM&quot;.&lt;&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;ap &lt;i&gt;or&lt;/i&gt; a&lt;/td&gt;&lt;td&gt;use am/pm display. &lt;b&gt;a/ap&lt;/b&gt; will be replaced by either &quot;am&quot; or &quot;pm&quot;.&lt;&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;t&lt;/td&gt;&lt;td&gt;the timezone (for example &quot;CEST&quot;)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;T&lt;/td&gt;&lt;td&gt;the offset from UTC&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;TT&lt;/td&gt;&lt;td&gt;the timezone IANA id&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;TTT&lt;/td&gt;&lt;td&gt;the timezone abbreviation&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;TTTT&lt;/td&gt;&lt;td&gt;the timezone short display name&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;TTTTT&lt;/td&gt;&lt;td&gt;the timezone long display name&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;TTTTTT&lt;/td&gt;&lt;td&gt;the timezone custom name. You can change it the &apos;Time zones&apos; tab of the configuration window&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;
&lt;p&gt;&lt;br /&gt;&lt;b&gt;Note:&lt;/b&gt; Any characters in the pattern that are not in the ranges of [&apos;a&apos;..&apos;z&apos;] and [&apos;A&apos;..&apos;Z&apos;] will be treated as quoted text. For instance, characters like &apos;:&apos;, &apos;.&apos;, &apos; &apos;, &apos;#&apos; and &apos;@&apos; will appear in the resulting time text even they are not enclosed within single quotes.The single quote is used to &apos;escape&apos; letters. Two single quotes in a row, whether inside or outside a quoted sequence, represent a &apos;real&apos; single quote.&lt;/p&gt;
</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>LxQtWorldClockConfigurationTimeZones</name>
    <message>
        <location filename="../lxqtworldclockconfigurationtimezones.ui" line="14"/>
        <source>World Clock Time Zones</source>
        <translation>Часовые пояса мирового времени</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfigurationtimezones.ui" line="36"/>
        <source>Time zone</source>
        <translation>Часовой пояс</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfigurationtimezones.ui" line="41"/>
        <source>Name</source>
        <translation>Название</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfigurationtimezones.ui" line="46"/>
        <source>Comment</source>
        <translation>Коментарий</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfigurationtimezones.ui" line="51"/>
        <source>Country</source>
        <translation>Страна</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfigurationtimezones.cpp" line="115"/>
        <source>UTC</source>
        <translation>UTC</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfigurationtimezones.cpp" line="118"/>
        <source>Other</source>
        <translation>Другое</translation>
    </message>
</context>
</TS>

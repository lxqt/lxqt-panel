<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru_RU">
<context>
    <name>LxQtWorldClockConfiguration</name>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="14"/>
        <source>World Clock Settings</source>
        <translation>Настройки мирового времени</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="24"/>
        <source>Time &amp;zones</source>
        <translation>Часовые &amp;пояса</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="42"/>
        <source>&amp;Add ...</source>
        <translation>&amp;Добавить…</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="52"/>
        <source>&amp;Remove</source>
        <translation>&amp;Удалить</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="62"/>
        <source>Set as &amp;default</source>
        <translation>Установить &amp;по-умолчанию</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="72"/>
        <source>Move &amp;up</source>
        <translation>&amp;Выше</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="82"/>
        <source>Move do&amp;wn</source>
        <translation>&amp;Ниже</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="105"/>
        <source>Display &amp;format</source>
        <translation>Формат &amp;отображения</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="123"/>
        <source>&amp;Short</source>
        <translation>&amp;Короткий</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="196"/>
        <source>&lt;h1&gt;Custom Date/Time Format Syntax&lt;/h1&gt;
&lt;p&gt;A date pattern is a string of characters, where specific strings of characters are replaced with date and time data from a calendar when formatting or used to generate data for a calendar when parsing.&lt;/p&gt;
&lt;p&gt;The Date Field Symbol Table below contains the characters used in patterns to show the appropriate formats for a given locale, such as yyyy for the year. Characters may be used multiple times. For example, if y is used for the year, &apos;yy&apos; might produce &apos;99&apos;, whereas &apos;yyyy&apos; produces &apos;1999&apos;. For most numerical fields, the number of characters specifies the field width. For example, if h is the hour, &apos;h&apos; might produce &apos;5&apos;, but &apos;hh&apos; produces &apos;05&apos;. For some characters, the count specifies whether an abbreviated or full form should be used, but may have other choices, as given below.&lt;/p&gt;
&lt;p&gt;Two single quotes represents a literal single quote, either inside or outside single quotes. Text within single quotes is not interpreted in any way (except for two adjacent single quotes). Otherwise all ASCII letter from a to z and A to Z are reserved as syntax characters, and require quoting if they are to represent literal characters. In addition, certain ASCII punctuation characters may become variable in the future (eg &amp;quot;:&amp;quot; being interpreted as the time separator and &apos;/&apos; as a date separator, and replaced by respective locale-sensitive characters in display).&lt;br /&gt;&lt;/p&gt;
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
&lt;tr&gt;&lt;td&gt;AP or A&lt;/td&gt;&lt;td&gt;use AM/PM display. &lt;b&gt;A/AP&lt;/b&gt; will be replaced by either &amp;quot;AM&amp;quot; or &amp;quot;PM&amp;quot;.&lt;&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;ap or a&lt;/td&gt;&lt;td&gt;use am/pm display. &lt;b&gt;a/ap&lt;/b&gt; will be replaced by either &amp;quot;am&amp;quot; or &amp;quot;pm&amp;quot;.&lt;&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;t&lt;/td&gt;&lt;td&gt;the timezone (for example &amp;quot;CEST&amp;quot;)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;T&lt;/td&gt;&lt;td&gt;the offset from UTC&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;TT&lt;/td&gt;&lt;td&gt;the timezone IANA id&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;TTT&lt;/td&gt;&lt;td&gt;the timezone abbreviation&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;TTTT&lt;/td&gt;&lt;td&gt;the timezone short display name&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;TTTTT&lt;/td&gt;&lt;td&gt;the timezone long display name&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;
&lt;p&gt;&lt;br /&gt;&lt;b&gt;Note:&lt;/b&gt; Any characters in the pattern that are not in the ranges of [&apos;a&apos;..&apos;z&apos;] and [&apos;A&apos;..&apos;Z&apos;] will be treated as quoted text. For instance, characters like &apos;:&apos;, &apos;.&apos;, &apos; &apos;, &apos;#&apos; and &apos;@&apos; will appear in the resulting time text even they are not enclosed within single quotes.The single quote is used to &apos;escape&apos; letters. Two single quotes in a row, whether inside or outside a quoted sequence, represent a &apos;real&apos; single quote.&lt;/p&gt;</source>
        <translation>&lt;h1&gt;Синтаксис своего формата даты/времени&lt;/h1&gt;
&lt;p&gt;Шаблон даты - это строка символов, где специальные группы символов заменяются при форматировании на дату и время из календаря или используются для генерации даты при синтаксическом разборе.&lt;/p&gt;
&lt;p&gt;Таблица символов полей даты ниже содержит символы, используемые в шаблонах для отображения соответствующих форматов установленной локали, акой как yyyy для года. Символы могут быть использованы несколько раз. Например, если использован y для года, &apos;yy&apos; отобразит &apos;99&apos;, тогда как &apos;yyyy&apos; покажет &apos;1999&apos;. Для большинства числовых полей число символов определяет ширину поля. Например, если h это час, &apos;h&apos; отобразит &apos;5&apos;, но &apos;hh&apos; отобразит &apos;05&apos;. Для некоторых символов, это число определяет должно ли быть использовано сокращение или полная форма, но могут быть и другие варианты, как написано ниже.&lt;/p&gt;
&lt;p&gt;Две одинарные кавычки отражают точно одиночную кавычку, независимо от того внутренние они или внешние. Текст внутри одинарных кавычек никак не интерпретируется (за исключением двух смежных одинарных кавычек). Тогда как все символы ASCII от a до z и A до Z зарезервированы под символы синтаксиса, и требуют заключения в кавычки для отображения их как обычных символов. омимо прочего, некоторые знаки препинания ASCII могут стать переменными в будущем (т.е. &amp;quot;:&amp;quot; быть интерпретированы как разделитель компонентов времени и &apos;/&apos; как разделитель компонентов даты, и заменены при отображении соответствующими символами с учётом локали ).&lt;br /&gt;&lt;/p&gt;
&lt;table border=&quot;1&quot; width=&quot;100%&quot; cellpadding=&quot;4&quot; cellspacing=&quot;0&quot;&gt;
&lt;tr&gt;&lt;th width=&quot;20%&quot;&gt;Код&lt;/th&gt;&lt;th&gt;Обозначения&lt;/th&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;d&lt;/td&gt;&lt;td&gt;день как число без первого нуля (от 1 до 31)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;dd&lt;/td&gt;&lt;td&gt;день как число с первым нулём (от 01 до 31)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;ddd&lt;/td&gt;&lt;td&gt;сокращённое локализованное название месяца (т.е. от &apos;Пн&apos; до &apos;Вс&apos;).&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;dddd&lt;/td&gt;&lt;td&gt;длинное локализованное название дня (т.е. от &apos;Понедельник&apos; до &apos;Вторник&apos;&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;M&lt;/td&gt;&lt;td&gt;месяц как число без первого нуля (1-12)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;MM&lt;/td&gt;&lt;td&gt;месяц как число с первым нулём (01-12)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;MMM&lt;/td&gt;&lt;td&gt;сокращённое локализованное название месяца (т.е. от &apos;Янв&apos; до &apos;Дек&apos;).&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;MMMM&lt;/td&gt;&lt;td&gt;длинное локализованное название месяца (т.е. от &apos;Январь&apos; до &apos;Декабрь&apos;).&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;yy&lt;/td&gt;&lt;td&gt;год как двухразрядное число (00-99)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;yyyy&lt;/td&gt;&lt;td&gt;год как четырёхразрядное число&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;h&lt;/td&gt;&lt;td&gt;час без нуля впереди (от 0 до 23 или 1 до 12, если отображается AM/PM)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;hh&lt;/td&gt;&lt;td&gt;час с нулём впереди (от 00 до 23 или 01 до 12, если отображается AM/PM)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;H&lt;/td&gt;&lt;td&gt;час без нуля впереди (от 00 до 23, даже с отображением AM/PM)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;HH&lt;/td&gt;&lt;td&gt;час с нулём впереди (от 00 до 23, даже с отображением AM/PM)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;m&lt;/td&gt;&lt;td&gt;минута без нуля впереди (от 0 до 59)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;mm&lt;/td&gt;&lt;td&gt;минута с нулём впереди (от 00 до 59)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;s&lt;/td&gt;&lt;td&gt;секунда без нуля впереди (от 0 до 59)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;ss&lt;/td&gt;&lt;td&gt;секунда с нулём впереди (от 00 до 59)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;z&lt;/td&gt;&lt;td&gt;миллисекунда без нуля впереди (от 0 до 999)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;zzz&lt;/td&gt;&lt;td&gt;миллисекунда с нулём впереди (от 000 до 999)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;AP or A&lt;/td&gt;&lt;td&gt;использовать отображение AM/PM. &lt;b&gt;A/AP&lt;/b&gt; будет замещено или &amp;quot;AM&amp;quot; или &amp;quot;PM&amp;quot;.&lt;&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;ap or a&lt;/td&gt;&lt;td&gt;использовать отображение am/pm. &lt;b&gt;a/ap&lt;/b&gt; будет замещено или &amp;quot;am&amp;quot; или &amp;quot;pm&amp;quot;.&lt;&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;t&lt;/td&gt;&lt;td&gt;временная зона (например &amp;quot;CEST&amp;quot;)&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;T&lt;/td&gt;&lt;td&gt;сдвиг времени от UTC&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;TT&lt;/td&gt;&lt;td&gt;id часового пояса IANA&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;TTT&lt;/td&gt;&lt;td&gt;аббревиатура часового пояса&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;TTTT&lt;/td&gt;&lt;td&gt;короткое имя часового пояса&lt;/td&gt;&lt;/tr&gt;
&lt;tr&gt;&lt;td&gt;TTTTT&lt;/td&gt;&lt;td&gt;длинное имя часового пояса&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;
&lt;p&gt;&lt;br /&gt;&lt;b&gt;Замечание:&lt;/b&gt; Любой символ в шаблоне не из диапазона [&apos;a&apos;..&apos;z&apos;] и [&apos;A&apos;..&apos;Z&apos;] будет обработан как цитируемый текст. Например, символы как &apos;:&apos;, &apos;.&apos;, &apos; &apos;, &apos;#&apos; и &apos;@&apos; появятся в тексте со временем, даже если они не помещены в одинарные кавычки. Одинарные кавычки используются для управляющих символ. Две одинарные кавычки подряд, независимо от того внутри или снаружи цитируемой последовательности, представляет &apos;реальные&apos; одинарные кавычки.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="133"/>
        <source>Lon&amp;g</source>
        <translation>Дли&amp;нный</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="140"/>
        <source>C&amp;ustom</source>
        <translation>С&amp;вой</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="252"/>
        <source>&amp;General</source>
        <translation>&amp;Общие</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="258"/>
        <source>Auto&amp;rotate when the panel is vertical</source>
        <translation>Авто&amp;поворот для вертикальной панели</translation>
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
</context>
</TS>

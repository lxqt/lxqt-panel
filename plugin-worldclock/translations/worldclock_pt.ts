<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="pt">
<context>
    <name>LxQtWorldClock</name>
    <message>
        <location filename="../lxqtworldclock.cpp" line="187"/>
        <source>&apos;&lt;b&gt;&apos;HH:mm:ss&apos;&lt;/b&gt;&lt;br/&gt;&lt;font size=&quot;-2&quot;&gt;&apos;ddd, d MMM yyyy&apos;&lt;br/&gt;&apos;TT&apos;&lt;/font&gt;&apos;</source>
        <translation>&apos;&lt;b&gt;&apos;HH:mm:ss&apos;&lt;/b&gt;&lt;br/&gt;&lt;font size=&quot;-2&quot;&gt;&apos;ddd, d MMM yyyy&apos;&lt;br/&gt;&apos;TT&apos;&lt;/font&gt;&apos;</translation>
    </message>
</context>
<context>
    <name>LxQtWorldClockConfiguration</name>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="14"/>
        <source>World Clock Settings</source>
        <translation>Definições do relógio mundial</translation>
    </message>
    <message>
        <source>&amp;Short, time only</source>
        <translation type="vanished">&amp;Curto, apenas horas</translation>
    </message>
    <message>
        <source>&amp;Long, time only</source>
        <translation type="vanished">&amp;Longo, apenas horas</translation>
    </message>
    <message>
        <source>S&amp;hort, date &amp;&amp; time</source>
        <translation type="vanished">C&amp;urto, hora e data</translation>
    </message>
    <message>
        <source>L&amp;ong, date &amp;&amp; time</source>
        <translation type="vanished">L&amp;ongo, hora e data</translation>
    </message>
    <message>
        <source>&amp;Custom</source>
        <translation type="vanished">&amp;Personalizar</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;Custom Date/Time Format Syntax&lt;/span&gt;&lt;/p&gt;&lt;p&gt;A date pattern is a string of characters, where specific strings of characters are replaced with date and time data from a calendar when formatting or used to generate data for a calendar when parsing.&lt;/p&gt;&lt;p&gt;The Date Field Symbol Table below contains the characters used in patterns to show the appropriate formats for a given locale, such as yyyy for the year. Characters may be used multiple times. For example, if y is used for the year, &apos;yy&apos; might produce &apos;99&apos;, whereas &apos;yyyy&apos; produces &apos;1999&apos;. For most numerical fields, the number of characters specifies the field width. For example, if h is the hour, &apos;h&apos; might produce &apos;5&apos;, but &apos;hh&apos; produces &apos;05&apos;. For some characters, the count specifies whether an abbreviated or full form should be used, but may have other choices, as given below.&lt;/p&gt;&lt;p&gt;Two single quotes represents a literal single quote, either inside or outside single quotes. Text within single quotes is not interpreted in any way (except for two adjacent single quotes). Otherwise all ASCII letter from a to z and A to Z are reserved as syntax characters, and require quoting if they are to represent literal characters. In addition, certain ASCII punctuation characters may become variable in the future (eg &amp;quot;:&amp;quot; being interpreted as the time separator and &apos;/&apos; as a date separator, and replaced by respective locale-sensitive characters in display).&lt;br/&gt;&lt;/p&gt;&lt;table border=&quot;1&quot; style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;&quot; width=&quot;100%&quot; cellspacing=&quot;0&quot; cellpadding=&quot;4&quot;&gt;&lt;tr&gt;&lt;td width=&quot;20%&quot;&gt;&lt;p align=&quot;center&quot;&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Code&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p align=&quot;center&quot;&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Meaning&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;d&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the day as number without a leading zero (1 to 31)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;dd&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the day as number with a leading zero (01 to 31)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;ddd&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the abbreviated localized day name (e.g. &apos;Mon&apos; to &apos;Sun&apos;).&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;dddd&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the long localized day name (e.g. &apos;Monday&apos; to &apos;Sunday&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;M&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the month as number without a leading zero (1-12)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;MM&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the month as number with a leading zero (01-12)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;MMM&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the abbreviated localized month name (e.g. &apos;Jan&apos; to &apos;Dec&apos;).&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;MMMM&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the long localized month name (e.g. &apos;January&apos; to &apos;December&apos;).&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;yy&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the year as two digit number (00-99)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;yyyy&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the year as four digit number&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;h&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;hh&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;H&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the hour without a leading zero (0 to 23, even with AM/PM display)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;HH&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the hour with a leading zero (00 to 23, even with AM/PM display)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;m&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the minute without a leading zero (0 to 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;mm&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the minute with a leading zero (00 to 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;s&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the second without a leading zero (0 to 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;ss&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the second with a leading zero (00 to 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;z&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the milliseconds without leading zeroes (0 to 999)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;zzz&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the milliseconds with leading zeroes (000 to 999)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;AP or A&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;use AM/PM display. &lt;span style=&quot; font-weight:600;&quot;&gt;A/AP&lt;/span&gt; will be replaced by either &amp;quot;AM&amp;quot; or &amp;quot;PM&amp;quot;.&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;ap or a&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;use am/pm display. &lt;span style=&quot; font-weight:600;&quot;&gt;a/ap&lt;/span&gt; will be replaced by either &amp;quot;am&amp;quot; or &amp;quot;pm&amp;quot;.&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;t&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the timezone (for example &amp;quot;CEST&amp;quot;)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;T&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the offset from UTC&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;TT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the timezone IANA id&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;TTT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the timezone abbreviation&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;TTTT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the timezone short display name&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;TTTTT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;the timezone long display name&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;p&gt;&lt;br/&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Note:&lt;/span&gt; Any characters in the pattern that are not in the ranges of [&apos;a&apos;..&apos;z&apos;] and [&apos;A&apos;..&apos;Z&apos;] will be treated as quoted text. For instance, characters like &apos;:&apos;, &apos;.&apos;, &apos; &apos;, &apos;#&apos; and &apos;@&apos; will appear in the resulting time text even they are not enclosed within single quotes.The single quote is used to &apos;escape&apos; letters. Two single quotes in a row, whether inside or outside a quoted sequence, represent a &apos;real&apos; single quote.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;Sintaxe do formato de data/hora personalizado&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Um formato de data é uma cadeia de caracteres em que os caracteres são substituidos pelos dados de data e hora de um calendário ou que são utilizados para gerar os dados do calendário.&lt;/p&gt;&lt;p&gt;A tabela de símbolos para campos de data abaixo contém os caracteres utilizados em padrões para mostrar os devidos formatos de uma região. Os caracteres podem ser utilizados diversas vezes. Por exemplo, se utilizar y para o anor, yy devolve 99 e yyyy devolve 1999. Para a maioria dos campo numéricos, o número de caracteres especifica o tamanho do campo. Por exemplo, se utilizar h para a hora, h devolve 5 mas hh devolve 05. Para alguns caracteres, o número de letras especifica o formato utilizado (pode ser abreviado ou completo), conforme explicado abaixo.&lt;/p&gt;&lt;p&gt;Duas aspas simples representam uma aspa simples literal, seja dentro ou fora das aspas simples. O texto entre aspas simples não é interpretado de qualquer forma (exceto para duas aspas simples adjacentes). Doutra forma, todas as letras ASCII , de &quot;a&quot; a &quot;z&quot; e &quot;A&quot; a &quot;Z&quot; estão reservadas para caracteres de sintaxe e são necessárias aspas para representarem caracteres literais. Adicionalmente, alguns símbolos de pontuação ASCII podem ser tornados variáveis no futuro (ex: &amp;quot;:&amp;quot; é interpretado como separador de hora e &apos;/&apos; como separador de data e são substituidos pelos separadores normalmente utilizados na região).&lt;br/&gt;&lt;/p&gt;&lt;table border=&quot;1&quot; style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;&quot; width=&quot;100%&quot; cellspacing=&quot;0&quot; cellpadding=&quot;4&quot;&gt;&lt;tr&gt;&lt;td width=&quot;20%&quot;&gt;&lt;p align=&quot;center&quot;&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Code&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p align=&quot;center&quot;&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Significado&lt;/span&gt;&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;d&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;o dia como número sem o zero inicial (1 a 31)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;dd&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;o dia como número mas com zero inicial (01 a 31)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;ddd&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;o nome do dia abreviado (Seg a Dom).&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;dddd&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;o nome do dia completo (Segunda a Domingo&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;M&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;o mês como número sem o zero inicial (1-12)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;MM&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;o mês como número mas com zero inicial (01-12)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;MMM&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;o nome abreviado do mês (Jan a Dez).&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;MMMM&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;o nome completo do mês (Janeiro a Dezembro).&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;yy&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;o ano como número de 2 dígitos (00-99)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;yyyy&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;o ano como número de 4 dígitos&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;h&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;a hora sem o zero inicial (0 a 23 ou 1 a 12 se AM/PM)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;hh&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;a hora mas com o zero inicial (00 a 23 ou 01 a 12 se AM/PM)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;H&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;a hora sem o zero inicial (0 a 23, mesmo se AM/PM)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;HH&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;a hora mas com o zero inicial (00 a 23, mesmo se AM/PM)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;m&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;os minutos sem o zero inicial (0 a 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;mm&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;os minutos mas com zero inicial (00 a 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;s&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;os segundos sem o zero inicial (0 a 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;ss&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;os segundos mas com zero inicial (00 a 59)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;z&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;os milissegundos sem o zero inicial (0 a 999)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;zzz&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;os milissegundos mas com zero inicial (000 a 999)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;AP ou A&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;para mostrar AM/PM &lt;span style=&quot; font-weight:600;&quot;&gt;A/AP&lt;/span&gt; será substituido por &amp;quot;AM&amp;quot; ou &amp;quot;PM&amp;quot;.&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;ap ou a&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;para mostrar am/pm &lt;span style=&quot; font-weight:600;&quot;&gt;a/ap&lt;/span&gt; será substituido por &amp;quot;am&amp;quot; ou &amp;quot;pm&amp;quot;.&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;t&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;o fuso horário (por exemplo: &amp;quot;CEST&amp;quot;)&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;T&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;o desvio da UTC&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;TT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;a ID IANA do fuso horário&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;TTT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;a abreviatura do fuso horário&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;TTTT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;o nome abreviado do fuso horário&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;p&gt;TTTTT&lt;/p&gt;&lt;/td&gt;&lt;td&gt;&lt;p&gt;o nome completo do fuso horário&lt;/p&gt;&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;p&gt;&lt;br/&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Nota:&lt;/span&gt; quaisquer caracteres no padrão que não estejam no intervalo [a...z] e [A...Z] serão tratados como texto. Por exemplo, os caracteres &apos;:&apos;, &apos;.&apos;, &apos; &apos;, &apos;#&apos; e &apos;@&apos; aparecerão no texto mesmo se não tiverem aspas.As aspas simples são utilizadas para &apos;escape&apos; de letras. As aspas duplas, dentro ou fora da sequência entre aspas, representa uma aspa simples &apos;real.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="403"/>
        <source>Time &amp;zones</source>
        <translation>Fusos &amp;horários</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="446"/>
        <source>&amp;Add ...</source>
        <translation>&amp;Adicionar...</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="456"/>
        <source>&amp;Remove</source>
        <translation>&amp;Remover</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="466"/>
        <source>Set as &amp;default</source>
        <translation>Utilizar como pré-&amp;definido</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="486"/>
        <source>Move &amp;up</source>
        <translation>Mover para &amp;cima</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="496"/>
        <source>Move do&amp;wn</source>
        <translation>Mover para &amp;baixo</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="24"/>
        <source>Display &amp;format</source>
        <translation>&amp;Formato de exibição</translation>
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
        <location filename="../lxqtworldclockconfiguration.ui" line="103"/>
        <source>&amp;Use 12-hour format</source>
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
        <location filename="../lxqtworldclockconfiguration.ui" line="196"/>
        <source>Location identifier</source>
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
        <location filename="../lxqtworldclockconfiguration.ui" line="431"/>
        <source>IANA id</source>
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
        <translation>&amp;Geral</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.ui" line="525"/>
        <source>Auto&amp;rotate when the panel is vertical</source>
        <translation>&amp;Rodar automaticamente se o painel estiver na vertical</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfiguration.cpp" line="105"/>
        <source>&apos;&lt;b&gt;&apos;HH:mm:ss&apos;&lt;/b&gt;&lt;br/&gt;&lt;font size=&quot;-2&quot;&gt;&apos;ddd, d MMM yyyy&apos;&lt;br/&gt;&apos;TT&apos;&lt;/font&gt;&apos;</source>
        <translation>&apos;&lt;b&gt;&apos;HH:mm:ss&apos;&lt;/b&gt;&lt;br/&gt;&lt;font size=&quot;-2&quot;&gt;&apos;ddd, d MMM yyyy&apos;&lt;br/&gt;&apos;TT&apos;&lt;/font&gt;&apos;</translation>
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
        <translation type="unfinished">Fusos horários do relógio mundial</translation>
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
        <translation>Fusos horários do relógio mundial</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfigurationtimezones.ui" line="36"/>
        <source>Time zone</source>
        <translation>Fuso horário</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfigurationtimezones.ui" line="41"/>
        <source>Name</source>
        <translation>Nome</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfigurationtimezones.ui" line="46"/>
        <source>Comment</source>
        <translation>Comentário</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfigurationtimezones.ui" line="51"/>
        <source>Country</source>
        <translation>País</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfigurationtimezones.cpp" line="115"/>
        <source>UTC</source>
        <translation>UTC</translation>
    </message>
    <message>
        <location filename="../lxqtworldclockconfigurationtimezones.cpp" line="118"/>
        <source>Other</source>
        <translation>Outro</translation>
    </message>
</context>
</TS>

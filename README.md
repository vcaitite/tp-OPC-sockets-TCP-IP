# tp_OPC_sockets_TCP-IP


<h1 align="center">
     Trabalho Prático sobre OPC e sockets TCP-IP</a>
</h1>

<p align="justify"> Esse trabalho visou colocar em prática boa parte do conteúdo aprendido durante as
aulas referentes a Sockets TCP/IP, Component Object Model e OPC da disciplina Sistemas Distribuídos para Automação. </p>

<p align="justify"> Normalmente o fluxo de minério de ferro nas ferrovias inicia com o envio de vagões
vazios aos pontos de carregamento. Para atingir os altos volumes de transporte, eficiência
e rapidez no carregamento é fundamental que esse ocorra de maneira automatizada, de
forma a se obter uma maior produtividade dos ativos. </p>

<p align="justify"> Uma maneira de automatizar essa tarefa é utilizando um sistema distribuído como
descrito nesse trabalho, que visa integrar sensores, um sistema de otimização, um software
que é responsável por levar os dados dos sensores até o sistema de otimização e levar
informações desse para um Controlador Lógico Programável, e obviamente esse CLP para
comandar o acionamento da comporta dos silos de carregamento e também a velocidade
dos vagões. </p>

<p align="justify"> O escopo desse trabalho é justamente desenvolver a aplicação capaz de integrar o
CLP e o sistema de otimização. Para isso, nossa aplicação funcionará como um servidor
de sockets, para se comunicar com o sistema de otimização (cliente de sockets), e como
cliente OPC, para interagir com o servidor OPC clássico que tem o acesso as variáveis de
processo do Controlador Lógico Programável. </p>


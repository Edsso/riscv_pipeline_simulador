#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

using namespace std;

struct Instruction {
    string full_instruction;
    string opcode;
    string rd;
    string rs1;
    string rs2;

    string to_string() const{
        string resul = "Intrucao Completa: " + full_instruction + "\t";
        resul += "Opcode: " + opcode + " | ";
        if (!rd.empty()) resul += "rd: " + rd + " | ";
        if (!rs1.empty()) resul += "rs1: " + rs1 + " | ";
        if (!rs2.empty()) resul += "rs2: " + rs2 + " | ";
        return resul;
    }
};

int mostrar_instrucoes(int instrucoes){
    return instrucoes;
}

// Função para converter hexadecimal para binário
string hexadecimal_para_binario(char hexadecimal) {
    switch (toupper(hexadecimal)) {
    case '0': return "0000";
    case '1': return "0001";
    case '2': return "0010";
    case '3': return "0011";
    case '4': return "0100";
    case '5': return "0101";
    case '6': return "0110";
    case '7': return "0111";
    case '8': return "1000";
    case '9': return "1001";
    case 'A': return "1010";
    case 'B': return "1011";
    case 'C': return "1100";
    case 'D': return "1101";
    case 'E': return "1110";
    case 'F': return "1111";
    default: return "Erro na conversao.";
    }
}

// Função para converter uma string hexadecimal inteira em binário
string hexadecimal_para_binario(string hexadecimal) {
    string binario;
    for (char c : hexadecimal)
        binario += hexadecimal_para_binario(c);
    return binario;
}

// Função para classificar as Intruçoes e Informar opcode, rd, rs1 e rs2
void decodificar_instrucao(const vector<Instruction> &instrucoes, map<string, int> &tipos_instrucoes){ 
    for (const auto &instrucao : instrucoes) {
        if (instrucao.full_instruction.length() == 32) {
            string opcode = instrucao.opcode;
            
            if (opcode == "0110011") {
                cout << "\n" << instrucao.to_string() << " ALU" << endl;
                tipos_instrucoes["ALU"]++;
            }
            else if (opcode == "1110011" || opcode == "0010011") {
                cout << "\n" << instrucao.to_string() << " ALU Immediate" << endl;
                tipos_instrucoes["ALU Immediate"]++;
            }
            else if (opcode == "0000011") {
                cout << "\n" << instrucao.to_string() << " Memory (Load)" << endl;
                tipos_instrucoes["Memory"]++;
            }
            else if (opcode == "0100011") {
                cout << "\n" << instrucao.to_string() << " Memory (Store)" << endl;
                tipos_instrucoes["Memory"]++;
            }
            else if (opcode == "1100011") {
                cout << "\n" << instrucao.to_string() << " Branch" << endl;
                tipos_instrucoes["Branch"]++;
            }
            else if (opcode == "0110111" || opcode == "0010111") {
                cout << "\n" << instrucao.to_string() << " Tipo Other (LUI/AUIPC)" << endl;
                tipos_instrucoes["Other"]++;
            }
            else if (opcode == "1101111") {
                cout << "\n" << instrucao.to_string() << " Jump (JAL)" << endl;
                tipos_instrucoes["Jump"]++;
            }
            else if (opcode == "1100111") {
                cout << "\n" << instrucao.to_string() << " Tipo Other (JALR)" << endl;
                tipos_instrucoes["Other"]++;
            }
            else {
                cout << "\nOpcode nao implementado: " << instrucao.full_instruction << endl;
            }
        } else {
            cout << "A instrucao " << instrucao.full_instruction << " nao tem 32 bits." << endl;
        }
    }
}

// Função para detalhar instrução a partir do formato binário
Instruction detalhar_instrucao(string instrucao_completa) {
    Instruction instrucao;
    instrucao.full_instruction = instrucao_completa;
    instrucao.opcode = instrucao_completa.substr(25, 7);

    if (instrucao.opcode == "0110011") { // Tipo R (ALU)
        instrucao.rd = instrucao_completa.substr(20, 5);
        instrucao.rs1 = instrucao_completa.substr(12, 5);
        instrucao.rs2 = instrucao_completa.substr(7, 5);
    } 
    else if (instrucao.opcode == "0010011" || instrucao.opcode == "1110011") { // Tipo I (Immediate)
        instrucao.rd = instrucao_completa.substr(20, 5);
        instrucao.rs1 = instrucao_completa.substr(12, 5);
    } 
    else if (instrucao.opcode == "0000011" || instrucao.opcode == "0100011") { // Tipo Memory (Load/Store)
        instrucao.rd = instrucao_completa.substr(20, 5);
        instrucao.rs1 = instrucao_completa.substr(12, 5);
    } 
    else if (instrucao.opcode == "1100011") { // Tipo B (Branch)
        instrucao.rs1 = instrucao_completa.substr(12, 5);
        instrucao.rs2 = instrucao_completa.substr(7, 5);
    } 
    else if (instrucao.opcode == "1101111") { // Tipo J (Jump)
        instrucao.rd = instrucao_completa.substr(20, 5);
    } 
    else if (instrucao.opcode == "0110111" || instrucao.opcode == "0010111") { // Other (LUI/AUIPC)
        instrucao.rd = instrucao_completa.substr(20, 5);
    } else {
        cout << "Opcode nao reconhecido: " << instrucao.opcode << endl;
    }
    return instrucao;
}

// Função para resolver conflitos de dados
void resolver_conflito_dados(const vector<Instruction>& instructions, vector<Instruction>& resolvido_instructions) {
    bool tem_conflito = false;
    bool next_tem_conflito = false;

    Instruction nop;
    nop.full_instruction = "00000000000000000000000000010011";

    for (int i = 0; i < instructions.size() - 1; i++) {
        
        const Instruction& current_inst = instructions[i];
        const Instruction& next_inst = instructions[i + 1];
        const Instruction& subsequent_inst = instructions[i + 2];

        resolvido_instructions.push_back(current_inst);
        
        tem_conflito = false;
        next_tem_conflito = false;

        if(current_inst.rd != "00000"){
            if(next_inst.rd == subsequent_inst.rs1 || next_inst.rd == subsequent_inst.rs2)
                next_tem_conflito = true;

            if(next_inst.opcode == "1110011" || next_inst.opcode == "0010011"){
                if (current_inst.rd == next_inst.rs1) { // Verificando se a próxima instrução tem conflito
                    tem_conflito = true;
                    cout << "\nConflito detectado na instrucao: " << current_inst.full_instruction;
                    resolvido_instructions.push_back(nop); // 2 nops para completar 2 ciclos antes da próxima instrução
                    resolvido_instructions.push_back(nop);
                } else if (!next_tem_conflito && !tem_conflito && current_inst.rd == subsequent_inst.rs1) { // Verificando se a instrução subsequente tem conflito
                    tem_conflito = true;
                    cout << "\nConflito detectado na instrucao: " << current_inst.full_instruction;
                    resolvido_instructions.push_back(nop); // 1 nop para instrução subsequente (1 nop + 1 instrução = 2 ciclos)
                }
            } else { // R etc
                if (current_inst.rd == next_inst.rs1 || current_inst.rd == next_inst.rs2) { // Verificando se a próxima instrução tem conflito
                    tem_conflito = true;
                    cout << "\nConflito detectado na instrucao: " << current_inst.full_instruction;
                    resolvido_instructions.push_back(nop); // 2 nops para completar 2 ciclos antes da próxima instrução
                    resolvido_instructions.push_back(nop);
                } else if (!next_tem_conflito && !tem_conflito && (current_inst.rd == subsequent_inst.rs1 || current_inst.rd == subsequent_inst.rs2)) { // Verificando se a instrução subsequente tem conflito
                    tem_conflito = true;
                    cout << "\nConflito detectado na instrucao: " << current_inst.full_instruction;
                    resolvido_instructions.push_back(nop); // 1 nop para instrução subsequente (1 nop + 1 instrução = 2 ciclos)
                }
            }
        }
        // forwarding
        if(current_inst.opcode == "0000011"){ // Load instructions
            if(next_inst.opcode == "1110011" || next_inst.opcode == "0010011" || next_inst.opcode == "0000011"){
                if(current_inst.rd == next_inst.rs1){
                    tem_conflito = true;
                    cout << "\nConflito detectado na instrucao Load: " << current_inst.full_instruction;
                    resolvido_instructions.push_back(nop);
                }
            } else if (next_inst.opcode == "1100011" || next_inst.opcode == "0100011" || next_inst.opcode == "0110011"){
                if(current_inst.rd == next_inst.rs1 || current_inst.rd == next_inst.rs2){
                    tem_conflito = true;
                    cout << "\nConflito detectado na instrucao Load: " << current_inst.full_instruction;
                    resolvido_instructions.push_back(nop);
                }
            }
        }
        if(!tem_conflito){
            cout << "\nSem conflito na instrucao: " << current_inst.full_instruction;
        }
    }
    resolvido_instructions.push_back(instructions[instructions.size() - 1]);
}

// Função para ler um arquivo de instruções
void ler_file(const string& input_file, vector<Instruction>& instructions) {
    fstream file(input_file);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            string instrucao_binaria = hexadecimal_para_binario(line);
            if (instrucao_binaria.length() == 32) {
                instructions.push_back(detalhar_instrucao(instrucao_binaria));
            }
        }
        file.close();
    } else {
        cout << "Nao foi possivel abrir o arquivo: " << input_file << endl;
    }
}

// Função para escrever as instruções resolvidas em um arquivo
void escrever_file(const vector<Instruction>& resolvido_instructions) {
    ofstream output_file("output.txt");
    if (output_file.is_open()) {
        for (const auto& instruction : resolvido_instructions) {
            output_file << instruction.full_instruction << "\n";
        }
        output_file.close();
    } else {
        cout << "Erro ao abrir o arquivo de saida." << endl;
    }
}

int main() {
    string input_file = "dumpfile2.txt";
    map<string, int> tipos_instrucoes = {{"ALU", 0}, {"ALU Immediate", 0}, {"Memory", 0}, {"Branch", 0}, {"Other", 0}, {"Jump", 0}};
    vector<Instruction> instructions;
    vector<Instruction> resolvido_instructions;

    // Leitura do arquivo
    ler_file(input_file, instructions);

    decodificar_instrucao(instructions, tipos_instrucoes);

    int total_intrucoes = 0;
        for (const auto &li : tipos_instrucoes){
            total_intrucoes += li.second;
        }

    cout << "<-------------------------------------------------->" << endl;

    cout << "Iniciando... ";

    cout << "\nClassificacao de Instrucoes: " << endl << endl;
    cout << "\tTotal de Instrucoes: " << mostrar_instrucoes(instructions.size()) << endl;
        for (const auto &li : tipos_instrucoes){
            cout << "\tTipo " << li.first << ": " << li.second << endl;
        }

    // Resolver conflitos de dados
    cout << "\nResolvendo conflitos de dados com NOPs..." << endl;
    resolver_conflito_dados(instructions, resolvido_instructions);
    cout << "\n\n\tTotal de Instrucoes com NOPs: " << mostrar_instrucoes(resolvido_instructions.size()) << endl;
    cout << "\tA quantidade de NOPs alocado na instrucao: " << mostrar_instrucoes(resolvido_instructions.size()-instructions.size()) << endl;

    // Escrever as instruções resolvidas no arquivo de saída
    cout << "\nAbrir arquivo 'output.txt' para visualizar os conflitos de dados resolvido com NOPs...\n";
    escrever_file(resolvido_instructions);

    cout << "<-------------------------------------------------->" << endl;

    return 0;
}

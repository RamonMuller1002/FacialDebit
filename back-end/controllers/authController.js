// controllers/authController.js
import pool from '../db.js';
import bcrypt from 'bcryptjs';
import jwt from 'jsonwebtoken';
import dotenv from 'dotenv';

dotenv.config();

// Registrar novo usuário
export const register = async (req, res) => {
  const { nome, email, senha } = req.body;

  try {
    // Verifica se o e-mail já existe
    const result = await pool.query('SELECT * FROM usuarios WHERE email = $1', [email]);
    if (result.rows.length > 0) {
      return res.status(400).json({ mensagem: 'Email já cadastrado' });
    }

    // Criptografa a senha
    const senhaHash = await bcrypt.hash(senha, 10);

    // Insere o usuário no banco
    await pool.query(
      'INSERT INTO usuarios (nome, email, senha) VALUES ($1, $2, $3)',
      [nome, email, senhaHash]
    );

    res.status(201).json({ mensagem: 'Usuário registrado com sucesso' });
  } catch (err) {
    res.status(500).json({ mensagem: 'Erro no registro', erro: err.message });
  }
};

// Login do usuário
export const login = async (req, res) => {
  const { email, senha } = req.body;

  try {
    const result = await pool.query('SELECT * FROM usuarios WHERE email = $1', [email]);

    if (result.rows.length === 0) {
      return res.status(400).json({ mensagem: 'Usuário não encontrado' });
    }

    const usuario = result.rows[0];

    // Compara senha
    const senhaCorreta = await bcrypt.compare(senha, usuario.senha);

    if (!senhaCorreta) {
      return res.status(401).json({ mensagem: 'Senha incorreta' });
    }

    // Gera token JWT
    const token = jwt.sign({ id: usuario.id }, process.env.JWT_SECRET, {
      expiresIn: '1h',
    });

    res.json({ mensagem: 'Login realizado com sucesso', token });
  } catch (err) {
    res.status(500).json({ mensagem: 'Erro no login', erro: err.message });
  }
};

export const view = async (req, res) => {
//debugger da conexão do banco de dados
console.log(pool);


  try {
    const result = await pool.query('SELECT * FROM usuarios');
    res.status(201).json({ result }); 
}
catch (err) {
    console.log(err);  
    res.status(500).json({ mensagem: 'Erro no registro', erro: err.message });
  }
};

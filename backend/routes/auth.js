// routes/auth.js
import express from 'express';
import { register, login, view } from '../controllers/authController.js';

const router = express.Router();

router.post('/register', register);
router.post('/login', login);
router.get('/view', view);

export default router;

# Valorix

Valorix is a collectible trading game where players unlock items, trade strategically, and build value over time. Smart deals, rare finds, and sharp timing separate casual collectors from true traders.

## What this project includes

- A React + Vite starter app.
- Firebase Google sign-in.
- A friendly greeting after login.
- A logout button.

## 1) Install and run the app

```bash
npm install
npm run dev
```

Vite will show a local URL like `http://localhost:5173`.

## 2) Add Firebase settings

1. Copy the example env file to a real one:

   ```bash
   cp .env.example .env
   ```

2. Open `.env` and replace each value with your Firebase web app values.

## 3) Firebase Console steps (Google login)

> You said you already have a Firebase project. Follow these steps in the Firebase Console to enable Google login and get the web app keys.

### A) Create a Web App (to get the config values)

1. Go to **Firebase Console** → select your project.
2. Click the **gear icon** → **Project settings**.
3. Scroll to **Your apps**.
4. Click the **Web** icon (`</>`).
5. Give it a nickname like `Valorix Web`.
6. Click **Register app**.
7. Copy the config values (apiKey, authDomain, projectId, etc.) into your `.env` file.

### B) Turn on Google sign-in

1. In Firebase Console, click **Build** → **Authentication**.
2. Click the **Sign-in method** tab.
3. Click **Google**.
4. Turn it **On**.
5. Choose your **Project support email**.
6. Click **Save**.

### C) Add localhost to authorized domains

1. In **Authentication**, open the **Settings** tab.
2. Under **Authorized domains**, make sure `localhost` is listed.

## 4) Run the app again

```bash
npm run dev
```

Click **Sign in with Google** and you should see “Hello [Player Name]”.

## File guide (simple explanations)

- `index.html` — The single HTML page Vite serves. It has the `<div id="root">` where React mounts the app.
- `src/main.jsx` — The entry point. It loads React, loads global styles, and renders `<App />`.
- `src/App.jsx` — The main UI. It shows the login button, greeting, and logout button.
- `src/firebase.js` — Firebase setup. Reads your `.env` values, creates the auth instance, and exposes helpers for login/logout.
- `src/index.css` — Styling for the simple layout.
- `.env.example` — A template of the Firebase environment variables you need to fill in.
- `vite.config.js` — Vite setup that enables React.
- `package.json` — Lists dependencies and the commands like `npm run dev`.

## Need help?

If you run into errors, double-check:
- `.env` values match the Firebase config exactly.
- Google Sign-in is enabled.
- You restarted `npm run dev` after changing `.env`.

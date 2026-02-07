import { useEffect, useState } from "react";
import { onAuthStateChanged } from "firebase/auth";
import {
  auth,
  googleProvider,
  signInWithGoogle,
  signOutUser,
} from "./firebase";

const greetingForUser = (user) => {
  if (!user) return "";
  return user.displayName || user.email || "Player";
};

export default function App() {
  const [user, setUser] = useState(null);
  const [status, setStatus] = useState("Checking login...");

  useEffect(() => {
    const unsubscribe = onAuthStateChanged(auth, (currentUser) => {
      setUser(currentUser);
      setStatus(currentUser ? "You are signed in." : "You are signed out.");
    });

    return () => unsubscribe();
  }, []);

  const handleLogin = async () => {
    setStatus("Opening Google login...");
    try {
      await signInWithGoogle(googleProvider);
    } catch (error) {
      console.error(error);
      setStatus("Login failed. Please try again.");
    }
  };

  const handleLogout = async () => {
    setStatus("Signing out...");
    try {
      await signOutUser();
    } catch (error) {
      console.error(error);
      setStatus("Logout failed. Please try again.");
    }
  };

  return (
    <main>
      <div className="container">
        <h1>Valorix</h1>
        <p>A simple player portal for the Valorix web game.</p>

        {user ? (
          <>
            <p>
              Hello <strong>{greetingForUser(user)}</strong> 👋
            </p>
            <button className="secondary" onClick={handleLogout}>
              Log out
            </button>
          </>
        ) : (
          <>
            <p>Sign in to see your player greeting.</p>
            <button className="primary" onClick={handleLogin}>
              Sign in with Google
            </button>
          </>
        )}

        <div className="status">{status}</div>
      </div>
    </main>
  );
}

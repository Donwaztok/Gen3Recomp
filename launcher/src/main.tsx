import React from "react";
import ReactDOM from "react-dom/client";
import App from "./App";
import "./index.css";

ReactDOM.createRoot(document.getElementById("root") as HTMLElement).render(
  <React.StrictMode>
    <App />
  </React.StrictMode>,
);

document.documentElement.classList.add("dark");
document.documentElement.setAttribute("data-theme", "dark");
document.body.classList.add("dark", "bg-background", "text-foreground");

package com.example.jnidemo;

import android.graphics.Color;
import android.os.Bundle;
import android.text.TextUtils;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    public native boolean isDebugDetected();
    public native String  helloFromJNI();
    public native int     factorial(int n);

    static {
        System.loadLibrary("native-lib");
    }

    private boolean environnementSur = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button   btnCheck    = findViewById(R.id.btnCheck);
        TextView tvStatus    = findViewById(R.id.tvStatus);

        Button   btnHello    = findViewById(R.id.btnHello);
        TextView tvHello     = findViewById(R.id.tvHello);

        EditText etFactInput = findViewById(R.id.etFactInput);
        Button   btnFact     = findViewById(R.id.btnFact);
        TextView tvFact      = findViewById(R.id.tvFact);

        btnCheck.setOnClickListener(v -> {
            boolean suspect = isDebugDetected();

            if (suspect) {
                environnementSur = false;
                tvStatus.setText("Etat securite : environnement suspect detecte");
                tvStatus.setTextColor(Color.RED);

                btnHello.setEnabled(false);
                etFactInput.setEnabled(false);
                btnFact.setEnabled(false);
                tvHello.setText("Fonction desactivee");
                tvFact.setText("Calcul bloque");
            } else {
                environnementSur = true;
                tvStatus.setText("Etat securite : OK");
                tvStatus.setTextColor(Color.parseColor("#2E7D32"));

                btnHello.setEnabled(true);
                etFactInput.setEnabled(true);
                btnFact.setEnabled(true);
            }
        });

        btnHello.setOnClickListener(v -> {
            if (!environnementSur) {
                Toast.makeText(this, "Environnement non verifie", Toast.LENGTH_SHORT).show();
                return;
            }
            tvHello.setText(helloFromJNI());
        });

        btnFact.setOnClickListener(v -> {
            if (!environnementSur) {
                Toast.makeText(this, "Environnement non verifie", Toast.LENGTH_SHORT).show();
                return;
            }
            String saisie = etFactInput.getText().toString().trim();
            if (TextUtils.isEmpty(saisie)) {
                Toast.makeText(this, "Entrez un entier", Toast.LENGTH_SHORT).show();
                return;
            }
            int n = Integer.parseInt(saisie);
            int res = factorial(n);
            if (res == -1) {
                tvFact.setText("Erreur : valeur negative non acceptee");
            } else if (res == -2) {
                tvFact.setText("Erreur : depassement pour n = " + n);
            } else {
                tvFact.setText("Factoriel de " + n + " = " + res);
            }
        });
    }
}
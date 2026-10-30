package com.aadil.mods;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.content.Intent;
import android.view.MenuItem;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        getActionBar().setTitle("Aadil Native Menu");
		System.loadLibrary("AadilMods");
        Main.Start(this);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return true;
    }
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		String url = null;

		switch (item.getItemId()) {
			case R.id.discord:
				url = "https://discord.gg/4X8Hhgwc5A";
				break;

			case R.id.telegram:
				url = "https://t.me/aadilmodsofficial";
				break;

			case R.id.instagram:
				url = "https://instagram.com/aadilmods";
				break;

			case R.id.website:
				url = "https://aadilmods.com";
				break;

			case R.id.github:
				url = "https://github.com/aadilmodsofficial";
				break;
		}

		if (url != null) {
			Intent intent = new Intent(Intent.ACTION_VIEW, android.net.Uri.parse(url));
			startActivity(intent);
			return true;
		}

		return super.onOptionsItemSelected(item);
	}
}
